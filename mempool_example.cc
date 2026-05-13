/**
 * This is a very simple memory pool demo.
 * Abstraction :
 *
 *
 *      + ---- [ free slot idx | free slot idx | free slot idx | ... ] => free slot subscript deque
 *      |
 *      V
 *      -----------------------------------------------------------
 *     |<---->|<---->|<---->|... => slots,constructed through vector<void *>
 *     [===========================================================] => underlying memory,acquired
 *                                                                      through ::operator new
 *                                                                      (not new[])
 *
 * The inner memory buffer's address will be aligned to 16 by default,but it is possible
 * to specify buffer address alignment explicitly through ctor.
 * The slot size will be known at the first time allocate method is called,and the size
 * will be computed to the next value that is power of 2.
 * Slot booking and free slot subscript deque will be constructed based on the slot size.
 * Free memory will return the slot idx to free slot subscript deque,no realy memory delete.
 * The memory pool descriptor will be wrapped by a shared pointer,and will be sent to the
 * objects they resided on this pool,to prevent early deallocate memory pool when there still
 * some objects are livingn,besides this,the descriptor will be used to invoke deallocate
 * method to return space.
 */

#include <new>
#include <memory>
#include <concepts>
#include <exception>
#include <vector>
#include <deque>
#include <print>
#include <bit>

#include <cstring>
#include <cstdint>

namespace mempool_ns {

  /**
   * requires_mempool - template concept to constraint
   *                    template type,it must supports
   *                    allocate(size_t) and deallocate(void *)
   *                    methods.
   * # simple concept.
   */
  template<typename _MemPoolType>
  concept requires_mempool = requires (_MemPoolType p,
                                       std::size_t size, void *address) {
    p.allocate(size);
    p.deallocate(address);
  };

  /**
   * RegisterOnPool<> - base class used to records which pool we
   *                    reside on
   * @_MempoolType:     template parameter,type of memory pool,must
   *                    satisfy the constraint
   * @shared_pool_:     share ownership to prevent the pool is
   *                    destroyed when we are not drained from the pool
   * @pool_registered():
   *                    member method,return the shared pointer reference
   * constructor:
   *                    explicit ctor(shared pointer<mempool type> p);
   *                    - requires explicitly specify memory pool.
   */
  template<requires_mempool _MempoolType>
  class RegisterOnPool {
  public:
    explicit RegisterOnPool(std::shared_ptr<_MempoolType> p)
      : shared_pool_(p)
    { }
    std::shared_ptr<_MempoolType> &pool_registered(void)
    {
      return shared_pool_;
    }
  private:
    std::shared_ptr<_MempoolType> shared_pool_;
  };


  /* memory pool exceptions */
  class mempool_except_bad_size : public std::bad_exception {
  public:
    virtual const char *what() const noexcept override
    {
      return "mempool error: bad object size.";
    }
  };

  class mempool_except_bad_alloc : public std::bad_exception {
  public:
    virtual const char *what() const noexcept override 
    {
      return "mempool error: no free space available.";
    }
  };

  class mempool_except_need_alloc : public std::bad_exception {
  public:
    virtual const char *what() const noexcept override
    {
      return "mempool error: must allocate object first.";
    }
  };

  class mempool_except_bad_dealloc : public std::bad_exception {
  public:
    virtual const char *what() const noexcept override
    {
      return "mempool error: address is not belong to mempool.";
    }
  };

  /**
   * Mempool - the memory pool class definition
   * # the minimal alignment in this pool is 4 bytes.          
   */
  class Mempool {
  public:
    /* default ctor */
    Mempool()
    {
      alignment_ = std::align_val_t{MEM_ALIGNMENT_VAL};
      __do_rest_init();
    }

    /* explicitly specify alignment */
    explicit Mempool(std::align_val_t alignment)
    {
      // ensure align
      if (static_cast<size_t>(alignment) < 4)
        alignment_ = std::align_val_t{4};
      else 
        alignment_ = std::align_val_t{std::bit_ceil(static_cast<size_t>(alignment))};
      __do_rest_init();
    }

    /**
     * allocate a free space,the final initialization is defer to
     * be executed in at the first time this routine is called.
     */
    void *allocate(std::size_t obj_size) noexcept(false)
    { 
      if (!obj_size)
        throw mempool_except_bad_size();

      if (!aligned_entity_size_)
        __do_final_init(obj_size);

      void *free_address = __next_free_slot();
      if (!free_address)
        throw mempool_except_bad_alloc();
      return free_address;
    }

    /* deallocate */
    void deallocate(void *address) noexcept(false)
    {
      if (!pool_size_)
        throw mempool_except_need_alloc();

      int its_idx(__find_address_return_idx(address));
      if (its_idx < 0)
        throw mempool_except_bad_dealloc();
      memset(address, 0, aligned_entity_size_);
      free_slots_.push_back(its_idx);
    }

    std::size_t pool_size()
    {
      return pool_size_;
    }

    /* before the final initialization,@pool_size_ is 0 */
    int in_use()
    {
      if (!pool_size_)
        return -1;
      return pool_size_ - free_slots_.size();
    }

    /* free slots subscripts all have been drained. */
    bool no_space()
    {
      return free_slots_.size() == 0;
    }

    ~Mempool()
    {
      ::operator delete(mem_buffer_);
    }

    void dump_pool(void)
    {
      std::println("dump memory pool : ");
      std::println("  Memory Pool : {:#x}", (unsigned long)this);
      std::println("  Size : {}", buffer_size_);
      std::println("  Buffer Alignment : {}", static_cast<size_t>(alignment_));
      std::println("  Entity Size(aligned) : {}", aligned_entity_size_);
      std::println("  Pool Size : {}", pool_size_);
      std::println("  In use : {}", in_use());
      std::println("  Is full ? - {}", no_space());
      std::println("[======== cut off ========]");
    }

  private:
    /**
     * __do_rest_init - allocate inner memory buffer,zero it out,and
     *                  initialize remaining data members
     */
    void __do_rest_init()
    {
      try {
        mem_buffer_ = ::operator new(MEM_BLOCK_SIZE, alignment_);
      } catch (std::bad_alloc&) {
        std::runtime_error("error: Initialize mempool failed.");
      }
      buffer_size_ = MEM_BLOCK_SIZE;
      memset(mem_buffer_, 0, buffer_size_);
      pool_size_ = 0;
      aligned_entity_size_ = 0;
    }


    /**
     * __do_final_init - build slot booking and free slot tracing
     *                   deque
     * @obj_size:        size of object
     * # the @aligned_entity_size_ will be the result that @obj_size
     *   align to alignment_.
     */
    void __do_final_init(std::size_t obj_size)
    {
      aligned_entity_size_ = aligned_size(obj_size);
      pool_size_ = buffer_size_ / aligned_entity_size_;
        
      // register slots with memory addresses.
      void *slot_address = mem_buffer_;
      for (unsigned int i(0); i < pool_size_; ++i) {
        slots_.push_back(static_cast<void *>(slot_address));
        free_slots_.push_back(i);
        slot_address += aligned_entity_size_;
      }
    }

    std::size_t aligned_size(std::size_t size)
    {
      return (size + static_cast<std::size_t>(alignment_) - 1)
        & ~(static_cast<std::size_t>(alignment_) - 1);
    }

    /**
     * linear find target address in slots.
     * if no such address is registered,return -1,
     * otherwise,return the subscript to slots.
     */
    int __find_address_return_idx(void *address)
    {
      std::size_t idx(0);
      for (auto e : slots_) {
        if (e == address)
          break;
        ++idx;
      }
      return idx == slots_.size() ? -1 : idx;
    }

    /* return the first available space */
    void *__next_free_slot()
    {
      if (no_space())
        return nullptr;
      std::size_t free_idx(free_slots_.front());
      free_slots_.pop_front();
      return slots_.at(free_idx);
    }

    /**
     * MEM_BLOCK_SIZE => size of inner memory buffer
     *                   (default 1024 bytes)
     * MEM_ALIGNMENT_VAL => address alignment of inner memory buffer
     *                      (default align to 16)
     */
    enum : std::size_t {
      MEM_BLOCK_SIZE = 1024,
      MEM_ALIGNMENT_VAL = 16,
    };

    /**
     * @mem_buffer_:        inner memory buffer
     * @alignment_:         address alignment for inner memory buffer
     * @buffer_size_:       inner memory buffer size
     * @pool_size_:         number of entities this pool is able to contains
     * @aligned_entity_size_:
     *                      aligned size of entity,this also is the slot size
     * @free_slots_:        deque<> to record free slots
     * @slots_:             entity slots collection,each element is a void pointer
     */
    void *mem_buffer_;
    std::align_val_t alignment_;
    std::size_t buffer_size_;
    std::size_t pool_size_;
    std::size_t aligned_entity_size_;
    std::deque<std::size_t> free_slots_;
    std::vector<void *> slots_;
  };

  static std::shared_ptr<Mempool> __mempool_create(std::align_val_t alignment)
  {
    std::shared_ptr<Mempool> pool_pointer = nullptr;
    try {
      pool_pointer = std::shared_ptr<Mempool>{new Mempool(alignment)};
    } catch (std::bad_alloc&) {
      std::runtime_error("error: failed to create mempool.");
    }
    return pool_pointer;
  }

  static inline std::shared_ptr<Mempool> mempool_create_a4(void)
  {
    return __mempool_create(std::align_val_t{4U});
  }

  static inline std::shared_ptr<Mempool> mempool_create_a8(void)
  {
    return __mempool_create(std::align_val_t{8U});
  }

  static inline std::shared_ptr<Mempool> mempool_create_a16(void)
  {
    return __mempool_create(std::align_val_t{16});
  }
}

using mempool_ns::RegisterOnPool;
using mempool_ns::Mempool;
using mempool_ns::mempool_create_a4;
using mempool_ns::mempool_create_a8;
using mempool_ns::mempool_create_a16;

class Testpool : public RegisterOnPool<Mempool> {
public:
  explicit Testpool(std::shared_ptr<Mempool> pool)
    : RegisterOnPool<Mempool>(pool)
  {
    a = 0;
    b = 0.0;
    c = '\0';
    x = 0;
  }

  void print_members(void)
  {
    std::println("a is {}, b is {}, c is {}, x is {}",
                 a, b, c, x);
  }

  void print_this(void)
  {
    std::println("this object at position {:#x}", (unsigned long)this);
  }

  void destroy(void)
  {
    auto pool = pool_registered();
    /* we need invoke dtor manually,
     * because we let it becomed private
     * member.
     * after the derived class's dtos is called,
     * then the base class RegisterOnPool<>'s
     * dtor is called,the shared pointer of pool held
     * by base class will be destructed.
     */
    this->~Testpool();
    pool->deallocate(this);
  }

private:
  ~Testpool()
  { }

  int a;
  float b;
  char c;
  void *x;
};

/* ctor of Testpool class */
template<typename _Tp>
static _Tp *create_object(std::shared_ptr<Mempool> pool) noexcept
{
  void *obj_pointer = nullptr;
  try {
    obj_pointer = pool->allocate(sizeof(_Tp));
  } catch (std::exception &pool_except) {
    std::println("{}", pool_except.what());
    return nullptr;
  }

  return new(obj_pointer) _Tp(pool);
}

int main(void)
{
  auto shared_ptr_pool(mempool_create_a16());
  shared_ptr_pool->dump_pool();

  try {
    void *tmp = shared_ptr_pool->allocate(0);
  } catch (std::exception& pool_except) {
    std::println("exception: {}", pool_except.what());
  }

  try {
    shared_ptr_pool->deallocate(0);
  } catch (std::exception& pool_except) {
    std::println("exception: {}", pool_except.what());
  }

  auto *testpool_obj(create_object<Testpool>(shared_ptr_pool));
  if (!testpool_obj) {
    std::println("create object failed.");
    shared_ptr_pool->dump_pool();
    return -1;
  }

  try {
    shared_ptr_pool->deallocate((void *)0xffffffff);
  } catch (std::exception& pool_except) {
    std::println("exception: {}", pool_except.what());
  }

  std::println("create object succeed.");
  std::println("sizeof object is {}", sizeof(Testpool));
  testpool_obj->print_members();
  testpool_obj->print_this();
  shared_ptr_pool->dump_pool();
  testpool_obj->destroy();
  shared_ptr_pool->dump_pool();

  /* no destroy,memory leak,just for test.
  while (1) {
    Testpool *for_test = nullptr;
    // should throw error.
    for_test = create_object<Testpool>(shared_ptr_pool);
    if (!for_test) {
      shared_ptr_pool->dump_pool();
      break;
    }
  }
  */

  return 0;
}

