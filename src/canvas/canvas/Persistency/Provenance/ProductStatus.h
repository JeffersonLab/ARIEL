#ifndef canvas_Persistency_Provenance_ProductStatus_h
#define canvas_Persistency_Provenance_ProductStatus_h

namespace art {

  using ProductStatus = unsigned char;

  namespace productstatus {
    constexpr ProductStatus
    present() noexcept
    {
      return 0x0;
    } // Product was made successfully
    constexpr ProductStatus
    neverCreated() noexcept
    {
      return 0x1;
    } // Product was not made successfully
    constexpr ProductStatus
    dropped() noexcept
    {
      return 0x2;
    } // Product was not made successfully
    constexpr ProductStatus
    dummyToPreventDoubleCount() noexcept
    {
      return 0x3;
    } // Product is a dummy placeholder object with an invalid range set to
      // prevent double-counting when combining run/subrun products
    constexpr ProductStatus
    unknown() noexcept
    {
      return 0xfe;
    } // Status unknown (used for backward compatibility)
    constexpr ProductStatus
    uninitialized() noexcept
    {
      return 0xff;
    } // Status not set

    constexpr bool
    present(ProductStatus const status) noexcept
    {
      return status == present();
    }
    constexpr bool
    neverCreated(ProductStatus const status) noexcept
    {
      return status == neverCreated();
    }
    constexpr bool
    dropped(ProductStatus const status) noexcept
    {
      return status == dropped();
    }
    constexpr bool
    dummyToPreventDoubleCount(ProductStatus const status) noexcept
    {
      return status == dummyToPreventDoubleCount();
    }
    constexpr bool
    unknown(ProductStatus const status) noexcept
    {
      return status == unknown();
    }
    constexpr bool
    uninitialized(ProductStatus const status) noexcept
    {
      return status == uninitialized();
    }
  } // productstatus
} // art

#endif /* canvas_Persistency_Provenance_ProductStatus_h */

// Local Variables:
// mode: c++
// End:
