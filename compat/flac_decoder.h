#pragma once

#include <micro_flac/flac_decoder.h>

namespace esp_audio_libs {
namespace flac {

enum FLACDecoderResult {
  FLAC_DECODER_SUCCESS = 0,
  FLAC_DECODER_HEADER_OUT_OF_DATA = 1,
  FLAC_DECODER_ERROR_OUT_OF_DATA = 1,
  FLAC_DECODER_ERROR = 2,
};

class FLACDecoder {
 public:
  void set_crc_check_enabled(bool enabled) { this->decoder_.set_crc_check_enabled(enabled); }

  FLACDecoderResult read_header(const uint8_t *buffer, size_t buffer_length) {
    size_t bytes_consumed = 0;
    size_t samples_decoded = 0;
    auto result =
        this->decoder_.decode(buffer, buffer_length, static_cast<uint8_t *>(nullptr), 0, bytes_consumed, samples_decoded);
    if (result == micro_flac::FLAC_DECODER_HEADER_READY) {
      return FLAC_DECODER_SUCCESS;
    }
    if (result == micro_flac::FLAC_DECODER_NEED_MORE_DATA) {
      return FLAC_DECODER_HEADER_OUT_OF_DATA;
    }
    return FLAC_DECODER_ERROR;
  }

  uint32_t get_sample_depth() const { return this->decoder_.get_stream_info().bits_per_sample(); }
  uint32_t get_num_channels() const { return this->decoder_.get_stream_info().num_channels(); }
  uint32_t get_sample_rate() const { return this->decoder_.get_stream_info().sample_rate(); }
  uint32_t get_output_buffer_size_bytes() const {
    const auto &info = this->decoder_.get_stream_info();
    return info.max_block_size() * info.num_channels() * info.bytes_per_sample();
  }

  FLACDecoderResult decode_frame(const uint8_t *buffer, size_t buffer_length, uint8_t *output_buffer,
                                 uint32_t *num_samples) {
    size_t bytes_consumed = 0;
    size_t samples_decoded = 0;
    auto result = this->decoder_.decode(buffer, buffer_length, output_buffer, this->get_output_buffer_size_bytes(),
                                        bytes_consumed, samples_decoded);
    *num_samples = samples_decoded;
    if (result == micro_flac::FLAC_DECODER_SUCCESS) {
      return FLAC_DECODER_SUCCESS;
    }
    if (result == micro_flac::FLAC_DECODER_NEED_MORE_DATA) {
      return FLAC_DECODER_ERROR_OUT_OF_DATA;
    }
    return FLAC_DECODER_ERROR;
  }

 protected:
  micro_flac::FLACDecoder decoder_;
};

}  // namespace flac
}  // namespace esp_audio_libs
