#pragma once

#include <memory>
#include <string>
#include <typeinfo>

namespace quatux
{
    namespace core
    {
        class buffer_t;

        namespace helpers
        {

            void mov_fw(const buffer_t *ptr, const std::size_t dest);
        }

        class buffer_t
        {
            buffer_t() = delete;
            buffer_t(const buffer_t& other) = delete;

        public:
            static std::shared_ptr<buffer_t> allocate(const std::size_t size);
            static std::shared_ptr<buffer_t> copy(const buffer_t& copy_from);

            uint8_t *data() const;

            std::size_t size() const;
            std::size_t pos() const;
            void reset_pos();

            template <typename T>
            void put(const T& item)
            {
                constexpr std::size_t item_type_size = sizeof(T);
                const std::size_t curr_position = pos();

                uint8_t *data_ptr = data();

                for (std::size_t i = 0; i < item_type_size; ++i)
                {
                    *(data_ptr + i) = static_cast<uint8_t>(item >> (i * 8));
                }

                helpers::mov_fw(this, item_type_size);
            }

            template <typename T>
            T get()
            {
                const std::size_t available_bytes = size() - pos();
                constexpr std::size_t item_type_size = sizeof(T);

                if (available_bytes < item_type_size)
                {
                    /*constexpr const std::string type_name = type_id(T).name();
                    const std::string exception_text = { "insufficient buffer available for type_name=" + type_name };*/
                    //throw std::overflow_error();
                }

                uint8_t *data_ptr = data();

                T val = {};

                for (std::size_t i = 0; i < item_type_size; ++i)
                {
                    T byte_val = (T)*(data_ptr + i);
                    val += (byte_val << (i * 8));
                }

                helpers::mov_fw(this, item_type_size);

                return val;
            }

            template <typename T>
            T get_at(const std::size_t item_index)
            {
                return T();                
            }
        };        
    }
}