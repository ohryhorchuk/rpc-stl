#include "buffer.h"

#include <stdexcept>
#include <sstream>
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

        void buffer_t::put(const std::string& str)
        {
            const std::size_t item_length = str.length();
            put(item_length);
            uint8_t *local_ptr = data();
            memcpy(local_ptr, str.c_str(), item_length);
            helpers::mov_fw(this, item_length);
        }

        void buffer_t::put(const buffer_t& from)
        {
            const std::size_t s = size();
            const std::size_t p = pos();

            const std::size_t from_s = size();
            const std::size_t from_p = pos();

            if ((s - p) < (from_s - from_p))
            {
                // exception
            }

            uint8_t *d = data();
            uint8_t *from_d = from.data();

            const std::size_t delta = from_s - from_p;
            memcpy(d, from_d, delta);
            helpers::mov_fw(this, delta);
        }

        std::string buffer_t::get()
        {
            const std::size_t upcomming_size = get<std::size_t>();
            std::stringstream ss;

            uint8_t *local_ptr = data();
            for (std::size_t i = 0; i < upcomming_size; ++i)
            {
                ss << local_ptr[i];
            }
            
            helpers::mov_fw(this, upcomming_size);
            return ss.str();
        }

        void buffer_t::get(std::shared_ptr<buffer_t>& dest)
        {
            std::size_t s = dest->size() - dest->pos();
            memcpy(dest->data(), data(), s);
            helpers::mov_fw(this, s);
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
            std::shared_ptr<buffer_t> other = allocate(copy_from.size());
            other->put(copy_from);
            other->reset_pos();
            return other;
        }
    }
}
