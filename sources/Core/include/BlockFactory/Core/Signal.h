/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * GNU Lesser General Public License v2.1 or any later version.
 */

#ifndef BLOCKFACTORY_CORE_SIGNAL_H
#define BLOCKFACTORY_CORE_SIGNAL_H

#include <memory>

namespace blockfactory {
    namespace core {
        class Signal;
        enum class DataType;
    } // namespace core
} // namespace blockfactory

/**
 * @brief Defines allowed signal data types
 *
 * This enum defines the data types of signals that are handled by this toolbox.
 *
 * @note Currently only `DOUBLE` is fully implemented.
 * @see core::Signal::Signal,
 *      core::BlockInformation::setInputPortType,
 *      core::BlockInformation::setOutputPortType
 */
enum class blockfactory::core::DataType
{
    DOUBLE,
    SINGLE,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    BOOLEAN,
};

/**
 * @brief Class to represent data shared by blocks, labelled as signals.
 *
 * Analogously to the block-algorithm corrispondence, this class introduces the signal-data
 * corrispondence. Signals are basically the connections between blocks.
 *
 * Signals do not directly translate to block's input and output. Signals are plugged into block
 * ports, and these block ports fill the signal with data.
 *
 * Briefly, core::Signal is a wrapper around a generic `void*` buffer.
 *
 * @remark A signal can be plugged to more than one block port.
 * @see core::Block
 */
class blockfactory::core::Signal
{
public:
    /// Defines the format of signals supported by core::Signal. It specifies how the object should
    /// be constructed depending on how the engine stores data flowing between the different blocks.
    ///
    /// @note DataFormat::CONTIGUOUS_ZEROCOPY is the only format that doesn't copy data from the
    ///       original buffer address into the core::Signal object. The engine is responsible of its
    ///       memory management, and a core::Signal object of this type can only access (either r/o
    ///       or r/w) to this raw buffer. On the other hand, both DataFormat::CONTIGUOUS and
    ///       DataFormat::NONCONTIGUOUS copy the content of the buffer inside the Signal object. For
    ///       performance reasons, prefer using DataFormat::CONTIGUOUS_ZEROCOPY.
    /// \par
    /// @note DataFormat::NONCONTIGUOUS is associated to r/o signals and methods as
    ///       core::Signal::setBuffer are not allowed.
    ///
    /// @see core::BlockInformation::getInputPortSignal,
    ///      core::BlockInformation::getOutputPortSignal
    ///
    enum class DataFormat
    {
        /// This data format matches the default behavior of Simulink input signals. The
        /// engine provides a pointer to an array of pointers (`void**`), each of them storing an
        /// element of the signal. The resulting core::Signal object will contain a copy of the
        /// content of the original buffer.
        ///
        /// @see core::Signal::initializeBufferFromNonContiguous
        ///
        NONCONTIGUOUS = 0,

        /// A core::Signal of this data format time is constructed from a pointer to a contiguous
        /// raw buffer (`void*`). The resulting core::Signal object will contain a copy of the
        /// content of the original buffer.
        ///
        /// @see core::Signal::initializeBufferFromNonContiguous
        ///
        CONTIGUOUS = 1,

        /// This data format matches the default behavior of Simulink output signals. The engine
        /// provides a pointer to a contiguous raw buffer (`void*`). The resulting core::Signal
        /// object, on the contrary of the other types, will not copy the content of the original
        /// buffer, and methods such as core::Signal::getBuffer will access data directly from the
        /// memory area of the engine.
        ///
        /// @see core::Signal::initializeBufferFromContiguousZeroCopy
        ///
        CONTIGUOUS_ZEROCOPY = 2
    };

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    class impl;
    std::unique_ptr<impl> pImpl;
#endif

public:
    enum
    {
        DynamicSize = -1
    };

    Signal(const DataFormat& dataFormat = DataFormat::CONTIGUOUS_ZEROCOPY,
           const DataType& dataType = DataType::DOUBLE);
    ~Signal();

    Signal(const Signal& other);
    Signal& operator=(const Signal& other) = delete;

    Signal(Signal&& other);
    Signal& operator=(Signal&& other) = delete;

    /**
     * @brief Initialize the signal from a contiguous buffer
     *
     * This method allocates a new array with the same size of `buffer` and copies the data. In this
     * case, the core::Signal object will own the data.
     *
     * @param buffer The pointer to the original contiguous buffer.
     * @return True for success, false otherwise.
     *
     * @see core::Signal::DataFormat
     */
    bool initializeBufferFromContiguous(const void* buffer);

    /**
     * @brief Initialize the signal from a contiguous buffer without copying data
     *
     * This methods accepts an external contiguous buffer and holds its pointer. The data is not
     * owned by this object.
     *
     * @note You must set the signal width with core::Signal::setWidth in order to have a valid
     *       signal.
     *
     * @param buffer The pointer to the original contiguous buffer.
     * @return True for success, false otherwise.
     *
     * @see core::Signal::DataFormat
     */
    bool initializeBufferFromContiguousZeroCopy(const void* buffer);

    /**
     * @brief Initialize the signal from a non-contiguous buffer
     *
     * This method allocates a new array with the same size of `bufferPtrs` and copies the data. In
     * this case, the core::Signal object will own the data. `bufferPtrs` points to an array of
     * pointers. Each of these pointers points to a data element.
     *
     * @note You must set the signal width with core::Signal::setWidth in order to have a valid
     *       signal.
     *
     * @param bufferPtrs The pointer to the original non-contiguous buffer.
     * @return True for success, false otherwise.
     *
     * @see core::Signal::DataFormat
     */
    bool initializeBufferFromNonContiguous(const void* const* bufferPtrs);

    /**
     * @brief Check if the signal is valid
     *
     * Checks if the internal buffer is not `nullptr` and the configured width is greater than zero.
     *
     * @return True for valid signal, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief Read the width of the signal
     *
     * By default the width of Signal is core::Signal::DynamicSize. However, for being a valid
     * signal, an object must have a specified width.
     *
     * @return The signal width.
     *
     * @see Signal::setWidth, Signal::isValid
     */
    int getWidth() const;

    /**
     * @brief Read the core::DataType of the signal
     *
     * The default type is core::DataType::DOUBLE.
     *
     * @return The signal data type.
     */
    DataType getPortDataType() const;

    /**
     * @brief Read the core::Signal::DataFormat of the signal
     *
     * The default type is core::DataFormat::CONTIGUOUS_ZEROCOPY.
     *
     * @return The signal data format.
     */
    DataFormat getDataFormat() const;

    /**
     * @brief Get the pointer to the buffer storing signal's data
     *
     * The buffer is stored as a void pointer. In order to use the buffer it should be properly cast
     * to the right data type. Be sure that the core::DataType matches the type of the buffer
     * otherwise pointer arithmetics does not work.
     *
     * If `T` does not match the configured data type, the returned value will be a `nullptr`.
     *
     * @tparam The data type of the returned buffer.
     * @return The pointer to the buffer if the class is properly configured, `nullptr` otherwise.
     *
     * @note Always check if the pointer is not `nullptr` before using it.
     * @see core::Signal::setBuffer
     */
    template <typename T>
    T* getBuffer();

    /**
     * @brief Get the pointer to the buffer storing signal's data
     *
     * Documented in core::Signal::getBuffer
     */
    template <typename T>
    const T* getBuffer() const;

    /**
     * @brief Get a single element of the signal
     *
     * This method returns the `i-th` element of the handled buffer.
     *
     * @tparam The data type of the returned signal. It must match configured data type.
     * @param i The index of the element. It should not exceed the configured width.
     * @return The `i-th` element of the vector if the signal is valid, or the default value of the
     *         type otherwise.
     *
     * @note It is recommended to use core::Signal::isValid before using this method.
     * @todo Switch to std::optional as soon as we switch to C++17
     */
    template <typename T>
    T get(const unsigned i) const;

    /**
     * @brief Set the width of the signal
     *
     * @param width The width to set.
     */
    void setWidth(const unsigned width);

    /**
     * @brief Set the value of a sigle element of the buffer
     *
     * @param index The index of the element to write.
     * @param data The content of the data to write.
     * @return True for success, false otherwise.
     *
     * @todo Port this to a template
     */
    bool set(const unsigned index, const double data);

    /**
     * @brief Set the pointer to the buffer storing signal's data
     *
     * This method allows changing the handled buffer. In the DataFormat::CONTIGUOUS case the data
     * is copied inside the object. Instead, in the DataFormat::CONTIGUOUS_ZEROCOPY only the pointer
     * to the buffer is changed.
     *
     * This method is not allowed for DataFormat::NONCONTIGUOUS.
     *
     * @tparam The data type of the new buffer.
     * @param data The new buffer address.
     * @param length The size of the new buffer.
     * @return True if the buffer was set sucessfully, false otherwise.
     */
    template <typename T>
    bool setBuffer(const T* data, const unsigned length);
};

// Explicit declaration of templates for all the supported types
// =============================================================

// TODO: for the time being, only DOUBLE is allowed. The toolbox has an almost complete support to
//       many other data types, but they need to be tested.

namespace blockfactory {
    namespace core {
        // DataType::DOUBLE
        extern template double* Signal::getBuffer<double>();
        extern template const double* Signal::getBuffer<double>() const;
        extern template double Signal::get<double>(const unsigned i) const;
        extern template bool Signal::setBuffer<double>(const double* data, const unsigned length);
    } // namespace core
} // namespace blockfactory

#endif // BLOCKFACTORY_CORE_SIGNAL_H
