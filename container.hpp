#pragma once

#include <string.h>
#include <type_traits>

class dynamic_array_storage {
  public:
    dynamic_array_storage(size_t element_size, size_t element_alignment)
      : element_size_{ element_size }
      , element_alignment_{ element_alignment }
      , elements_{ nullptr }
      , current_elements_{ 0 }
      , maximum_elements_{ 0 } {
    }

    ~dynamic_array_storage() {
      maximum_elements_ = 0;
      current_elements_ = 0;

      if(elements_ != nullptr) {
        free(elements_);
        elements_ = nullptr;
      }

      element_alignment_ = 0;
      element_size_      = 0;
    }

    template <typename element_t>
    element_t* begin() {
      return reinterpret_cast<element_t*>(elements_);
    }

    template <typename element_t>
    element_t* end() {
      auto used_bytes = current_elements_ * element_size_;

      return reinterpret_cast<element_t*>(elements_ + used_bytes);
    }

    template <typename element_t>
    void push_back(element_t&& value) {
      if(current_elements_ == maximum_elements_) {
        grow<element_t>();
      }

      *(begin<element_t>() + current_elements_) =
          std::forward<element_t>(value);

      current_elements_++;
    }

  private:
    template <typename element_t>
    void grow() {
      auto old_elements         = elements_;
      auto old_maximum_elements = maximum_elements_;

      if(old_elements == nullptr) {
        maximum_elements_ = 1;
      }

      maximum_elements_ *= 2;

      elements_ =
          reinterpret_cast<uint8_t*>(malloc(maximum_elements_ * element_size_));
      memset(elements_, 0, maximum_elements_ * element_size_);
      if(old_elements != nullptr) {
        memcpy(elements_, old_elements, old_maximum_elements * element_size_);
        free(old_elements);
      }
    }

  private:
    size_t   element_size_;
    size_t   element_alignment_;
    uint8_t* elements_;
    size_t   current_elements_;
    size_t   maximum_elements_;
};

template <typename element_t>
class dynamic_array_view {
  public:
    dynamic_array_view(dynamic_array_storage& storage)
      : storage_{ storage } {
    }

    element_t* begin() {
      return storage_.begin<element_t>();
    }

    element_t* end() {
      return storage_.end<element_t>();
    }

    void push_back(element_t&& value) {
      storage_.push_back<element_t>(std::forward<element_t>(value));
    }

  private:
    dynamic_array_storage& storage_;
};
