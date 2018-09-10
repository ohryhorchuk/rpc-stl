#include "buffer.h"

#include <stdexcept>

namespace quatux
{
    namespace core
    {
        namespace helpers
        {
            constexpr auto header_size = sizeof(std::size_t) * 2;

            void free_buffer(buffer_t* buf)
            {
                delete[] reinterpret_cast<uint8_t*>(buf);
            }

            std::size_t* as_size_t_ptr(const buffer_t *ptr)
            {
                return reinterpret_cast<std::size_t*>(const_cast<buffer_t *>(ptr));
            }

            uint8_t *to_uint8_t_ptr(const buffer_t *ptr)
            {
                return reinterpret_cast<uint8_t*>(const_cast<buffer_t *>(ptr));
            }

            uint8_t *to_uint8_t_without_headr_ptr(const buffer_t *ptr)
            {
                return to_uint8_t_ptr(ptr) + header_size;
            }

            void init_with_size_and_zero_pos(buffer_t *ptr, const std::size_t size)
            {
                std::size_t* local_ptr = as_size_t_ptr(ptr);
                local_ptr[0] = size;
                local_ptr[1] = 0;
            }

            void mov_fw(const buffer_t *ptr, const std::size_t dest)
            {
                as_size_t_ptr(ptr)[1] += dest;
            }

            std::size_t pos(const buffer_t *ptr)
            {
                return as_size_t_ptr(ptr)[1];
            }

            void pos(const buffer_t *ptr, const std::size_t new_pos)
            {
                as_size_t_ptr(ptr)[1] = new_pos;
            }

            std::size_t size(const buffer_t *ptr)
            {
                return as_size_t_ptr(ptr)[0];
            }
        }

        uint8_t *buffer_t::data() const
        {
            return helpers::to_uint8_t_without_headr_ptr(this) + pos();;
        }

        std::size_t buffer_t::pos() const
        {
            return helpers::pos(this);
        }

        void buffer_t::reset_pos()
        {
            helpers::pos(this, 0);
        }

        std::size_t buffer_t::size() const
        {
            return helpers::size(const_cast<buffer_t*>(this));
        }

        std::shared_ptr<buffer_t> buffer_t::allocate(const std::size_t size)
        {
            if (size >= std::numeric_limits<std::size_t>::max())
            {
                throw std::out_of_range("buffer_t::allocate: size exceeds the max size that quatux::core::buffer_t could support");
            }

            std::shared_ptr<buffer_t> buffer(reinterpret_cast<buffer_t*>(new uint8_t[size + helpers::header_size]), &helpers::free_buffer);
            helpers::init_with_size_and_zero_pos(buffer.get(), size);

            return buffer;
        }

        std::shared_ptr<buffer_t> buffer_t::copy(const buffer_t& copy_from)
        {
            return nullptr;
        }
    }
}
