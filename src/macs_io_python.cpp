#pragma push_macro("slots")
#undef slots

#include <pybind11/pybind11.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>

#pragma pop_macro("slots")

#include "macs_io.h"

namespace py = pybind11;

std::string getImageType(int number) {
    // find type
    int imgTypeInt = number % 8;
    std::string imgTypeString;

    switch (imgTypeInt) {
        case 0:
            imgTypeString = "8U";
            break;
        case 1:
            imgTypeString = "8S";
            break;
        case 2:
            imgTypeString = "16U";
            break;
        case 3:
            imgTypeString = "16S";
            break;
        case 4:
            imgTypeString = "32S";
            break;
        case 5:
            imgTypeString = "32F";
            break;
        case 6:
            imgTypeString = "64F";
            break;
        default:
            break;
    }

    // find channel
    int channel = (number / 8) + 1;

    std::stringstream type;
    type << "CV_" << imgTypeString << "C" << channel;

    return type.str();
}

PYBIND11_MODULE(macs_io_python, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    py::class_<MACS::CorrectionOptions::Stretch>(m, "CorrectionOptionsStretch")
            .def(py::init<>())
            .def_readwrite("gamma", &MACS::CorrectionOptions::Stretch::gamma)
            .def_readwrite("min", &MACS::CorrectionOptions::Stretch::min)
            .def_readwrite("max", &MACS::CorrectionOptions::Stretch::max);

    py::class_<MACS::CorrectionOptions::Devignetting>(m, "CorrectionOptionsDevignetting")
            .def(py::init<>())
            .def_readwrite("offset", &MACS::CorrectionOptions::Devignetting::offset)
            .def_readwrite("factor", &MACS::CorrectionOptions::Devignetting::factor)
            .def_readwrite("a", &MACS::CorrectionOptions::Devignetting::a)
            .def_readwrite("b", &MACS::CorrectionOptions::Devignetting::b)
            .def_readwrite("c", &MACS::CorrectionOptions::Devignetting::c)
            .def_readwrite("cx", &MACS::CorrectionOptions::Devignetting::cx)
            .def_readwrite("cy", &MACS::CorrectionOptions::Devignetting::cy);

    py::class_<MACS::CorrectionOptions::ColorBalance>(m, "CorrectionOptionsColorBalance")
            .def(py::init<>())
            .def_readwrite("r", &MACS::CorrectionOptions::ColorBalance::r)
            .def_readwrite("g", &MACS::CorrectionOptions::ColorBalance::g)
            .def_readwrite("b", &MACS::CorrectionOptions::ColorBalance::b);

    py::class_<MACS::CorrectionOptions::Distortion>(m, "CorrectionOptionsDistortion")
            .def(py::init<>())
            .def_readwrite("cx_px", &MACS::CorrectionOptions::Distortion::cx_px)
            .def_readwrite("cy_px", &MACS::CorrectionOptions::Distortion::cy_px)
            .def_readwrite("k1", &MACS::CorrectionOptions::Distortion::k1)
            .def_readwrite("k2", &MACS::CorrectionOptions::Distortion::k2)
            .def_readwrite("k3", &MACS::CorrectionOptions::Distortion::k3);

    py::class_<MACS::CorrectionOptions>(m, "CorrectionOptions")
            .def(py::init<>())
            .def_readwrite("stretch", &MACS::CorrectionOptions::stretch)
            .def_readwrite("devignetting", &MACS::CorrectionOptions::devignetting)
            .def_readwrite("color_balance", &MACS::CorrectionOptions::color_balance)
            .def_readwrite("distortion", &MACS::CorrectionOptions::distortion)
            .def_readwrite("convertTo8bit", &MACS::CorrectionOptions::convertTo8bit);

    py::enum_<MACS::MacsImageFormatVersion>(m, "MacsImageFormatVersion")
            .value("Version_1", MACS::MacsImageFormatVersion::Version_1)
            .value("Version_2", MACS::MacsImageFormatVersion::Version_2)
            .export_values();

    py::enum_<MACS::PixelFormat>(m, "MacsPixelFormat")
            .value("Mono12Packed", MACS::PixelFormat::Mono12Packed)
            .value("BayerGR12Packed", MACS::PixelFormat::BayerGR12Packed)
            .value("BayerBG12Packed", MACS::PixelFormat::BayerBG12Packed)
            .value("BayerGB12Packed", MACS::PixelFormat::BayerGB12Packed)
            .value("BayerRG12Packed", MACS::PixelFormat::BayerRG12Packed)
            .value("Mono16", MACS::PixelFormat::Mono16)
            .value("BayerGR16", MACS::PixelFormat::BayerGR16)
            .value("BayerBG16", MACS::PixelFormat::BayerBG16)
            .value("BayerRG16", MACS::PixelFormat::BayerRG16)
            .value("BayerGB16", MACS::PixelFormat::BayerGB16)
            .value("INVALID", MACS::PixelFormat::INVALID)
            .export_values();

    py::enum_<MACS::PixelEndianness>(m, "MacsPixelEndianness")
            .value("Undefined", MACS::PixelEndianness::Undefined)
            .value("Big", MACS::PixelEndianness::Big)
            .value("Little", MACS::PixelEndianness::Little)
            .export_values();

    auto macsImageData = py::class_<MACS::MacsImageData>(m, "MacsImageData");
    macsImageData.def(py::init<>());
    macsImageData.def(
            "data",
            [](MACS::MacsImageData& data) {
                auto byteArray = data.data();
                py::buffer_info buffer_info(reinterpret_cast<std::uint8_t*>(byteArray.data()),
                                            sizeof(std::uint8_t),
                                            py::format_descriptor<std::uint8_t>::format(),
                                            1,
                                            {byteArray.size()},
                                            {sizeof(std::uint8_t)});
                return py::array_t<std::uint8_t>(buffer_info);
            },
            py::return_value_policy::copy);

    macsImageData.def("rawData", [](MACS::MacsImageData& data) {
        py::buffer_info buffer_info(reinterpret_cast<std::uint8_t*>(data.rawData().data()),
                                    sizeof(std::uint8_t),
                                    py::format_descriptor<const std::uint8_t>::format(),
                                    1,
                                    {data.rawData().size()},
                                    {sizeof(std::uint8_t)});
        return py::array_t<std::uint8_t>(buffer_info);
    }, py::return_value_policy::copy);

    macsImageData.def("width", &MACS::MacsImageData::width);
    macsImageData.def("height", &MACS::MacsImageData::height);
    macsImageData.def("pitch", &MACS::MacsImageData::pitch);
    macsImageData.def("format", &MACS::MacsImageData::format);
    macsImageData.def("endianness", &MACS::MacsImageData::endianness);
    macsImageData.def("byteSize", &MACS::MacsImageData::byteSize);
    macsImageData.def("isValid", &MACS::MacsImageData::isValid);
    macsImageData.def("isMono", &MACS::MacsImageData::isMono);
    macsImageData.def("isColor", &MACS::MacsImageData::isColor);
    macsImageData.def("bitDepth", &MACS::MacsImageData::bitDepth);

    macsImageData.def("init",
                      [&](MACS::MacsImageData& imgData, py::array_t<std::uint8_t, py::array::c_style> pyBuffer, int pixelWidth, int pixelHeight, int pixelPitch, MACS::PixelFormat format,
                         MACS::PixelEndianness endianness) {
                          py::buffer_info bufferInfo = pyBuffer.request();
                          QByteArray array(static_cast<char*>(bufferInfo.ptr), bufferInfo.size * bufferInfo.itemsize);
                          MACS::log(MACS::Log::Error) << bufferInfo.size << bufferInfo.itemsize << bufferInfo.format;
                          imgData = MACS::MacsImageData(array, pixelWidth, pixelHeight, pixelPitch, format, endianness);
                      });


    auto poseEvent = py::class_<MACS::PoseEvent>(m, "PoseEvent");
    poseEvent.def_readwrite("roll", &MACS::PoseEvent::roll);
    poseEvent.def_readwrite("pitch", &MACS::PoseEvent::pitch);
    poseEvent.def_readwrite("yaw", &MACS::PoseEvent::yaw);
    poseEvent.def_readwrite("lat", &MACS::PoseEvent::lat);
    poseEvent.def_readwrite("lon", &MACS::PoseEvent::lon);
    poseEvent.def_readwrite("alt", &MACS::PoseEvent::alt);
    poseEvent.def_readwrite("veln", &MACS::PoseEvent::veln);
    poseEvent.def_readwrite("vele", &MACS::PoseEvent::vele);
    poseEvent.def_readwrite("velup", &MACS::PoseEvent::velup);
    poseEvent.def_property("time", [](const MACS::PoseEvent& e) {
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(e.time.toMSecsSinceEpoch()));
    }, [](MACS::PoseEvent& e, std::chrono::system_clock::time_point timepoint) {
        e.time = QDateTime::fromMSecsSinceEpoch(std::chrono::duration_cast<std::chrono::milliseconds>(timepoint.time_since_epoch()).count());
    });
    poseEvent.def("isValid", [](const MACS::PoseEvent& e) {
        return e.time.isValid();
    });

    auto macsMeta = py::class_<MACS::MacsMetaData>(m, "MacsMetaData");
    macsMeta.def(py::init<>());
    macsMeta.def_property(
            "camVendor",
            [](const MACS::MacsMetaData& meta) { return meta.camVendor.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camVendor.fromStdString(value); });
    macsMeta.def_property(
            "camModel",
            [](const MACS::MacsMetaData& meta) { return meta.camModel.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camModel.fromStdString(value); });
    macsMeta.def_property(
            "camName",
            [](const MACS::MacsMetaData& meta) { return meta.camName.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camName.fromStdString(value); });
    macsMeta.def_property(
            "camSerial",
            [](const MACS::MacsMetaData& meta) { return meta.camSerial.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camSerial.fromStdString(value); });
    macsMeta.def_property(
            "camMAC",
            [](const MACS::MacsMetaData& meta) { return meta.camMAC.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camMAC.fromStdString(value); });
    macsMeta.def_property(
            "camIP",
            [](const MACS::MacsMetaData& meta) { return meta.camIP.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camIP.fromStdString(value); });
    macsMeta.def_property(
            "comment",
            [](const MACS::MacsMetaData& meta) { return meta.comment.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.comment.fromStdString(value); });
    macsMeta.def_property(
            "camFirmware",
            [](const MACS::MacsMetaData& meta) { return meta.camFirmware.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.camFirmware.fromStdString(value); });
    macsMeta.def_property(
            "affix",
            [](const MACS::MacsMetaData& meta) { return meta.affix.toStdString(); },
            [](MACS::MacsMetaData& meta, const std::string& value) { meta.affix.fromStdString(value); });
    macsMeta.def_property(
            "imageID",
            [](const MACS::MacsMetaData& meta) { return meta.imageID; },
            [](MACS::MacsMetaData& meta, std::int32_t value) { meta.imageID = value; });
    macsMeta.def_property(
            "tapCount",
            [](const MACS::MacsMetaData& meta) { return meta.tapCount; },
            [](MACS::MacsMetaData& meta, std::int32_t value) { meta.tapCount = value; });
    macsMeta.def_property(
            "imageIDX",
            [](const MACS::MacsMetaData& meta) { return meta.imageIDX; },
            [](MACS::MacsMetaData& meta, std::int32_t value) { meta.imageIDX = value; });
    macsMeta.def_property(
            "expTimeUS",
            [](const MACS::MacsMetaData& meta) { return meta.expTimeUS; },
            [](MACS::MacsMetaData& meta, std::int32_t value) { meta.expTimeUS = value; });
    macsMeta.def_property(
            "timeStamp",
            [](const MACS::MacsMetaData& meta) { return meta.timeStamp; },
            [](MACS::MacsMetaData& meta, std::int32_t value) { meta.timeStamp = value; });

    auto macsImage = py::class_<MACS::MacsImage>(m, "MacsImage");
    macsImage.def(py::init<>());
    macsImage.def("load", [](MACS::MacsImage& img, std::string filename) {
        return img.load(filename.c_str());
    });
    macsImage.def("getCorrectedImage", [](const MACS::MacsImage& img, const MACS::CorrectionOptions& options) {
        auto corrected = img.getCorrectedImage(options);
        int depth = corrected.elemSize1();
        auto buffer_info = py::buffer_info(
                // Pointer to buffer
                corrected.data,
                // Size of one scalar
                depth,
                // Python struct-style format descriptor
                depth == 1 ? pybind11::format_descriptor<std::uint8_t>::format() : pybind11::format_descriptor<std::uint16_t>::format(),
                // Number of dimensions
                3,
                // Buffer dimensions
                {corrected.rows, corrected.cols, corrected.channels()},
                // Strides (in bytes) for each index
                {
                        (size_t) corrected.step,
                        (size_t) depth * corrected.channels(),
                        (size_t) depth
                }
        );
        return py::array_t<std::uint8_t>(buffer_info);
    });
    macsImage.def("save", [](MACS::MacsImage& image, std::string filename, bool preview, bool compression) {
        return image.save(filename.c_str(), preview, compression);
    });
    macsImage.def_readwrite("imageData", &MACS::MacsImage::imageData);
    macsImage.def_readwrite("metaData", &MACS::MacsImage::meta);
    macsImage.def_readwrite("geoPose", &MACS::MacsImage::georef);
}