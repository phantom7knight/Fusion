#pragma once

#include "../CorePCH.hpp"

// Collection of free utility functions.

namespace donut::Utilities
{
	class StoreAndResetBool {
	public:
		// Constructor takes a reference to an external boolean variable and updates the external variable
		StoreAndResetBool(bool& aVariableRef, bool aNewValue) : mVariableRef(aVariableRef), mOriginalValue(aVariableRef)
        {
            mVariableRef = aNewValue;
        }

		// Destructor resets the external variable to the original value
		~StoreAndResetBool() {
			mVariableRef = mOriginalValue;
		}

	private:
		bool& mVariableRef;       // Reference to the external boolean variable
		const bool mOriginalValue; // Original value of the external variable
	};

    inline void FatalError(const std::string_view message,
        const std::source_location sourceLocation = std::source_location::current())
    {
        const std::string errorMessage =
            std::format("[FATAL ERROR] :: {}. Source Location data : File Name -> {}, Function Name -> "
                "{}, Line Number -> {}, Column -> {}.\n",
                message, sourceLocation.file_name(), sourceLocation.function_name(), sourceLocation.line(),
                sourceLocation.column());

        throw std::runtime_error(errorMessage);
    }

    inline void ThrowIfFailed(const HRESULT hr, const std::source_location sourceLocation = std::source_location::current())
    {
        if (FAILED(hr))
        {
            FatalError("HRESULT failed!", sourceLocation);
        }
    }

    template <typename T>
    static inline constexpr typename std::underlying_type<T>::type enumClassValue(const T& value)
    {
        return static_cast<std::underlying_type<T>::type>(value);
    }
};