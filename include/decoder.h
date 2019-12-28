#ifndef DECODER_H
#define DECODER_H
#include<iostream>
#include<string>
#include<fstream>
#include<iterator>
#include<vector>
#include<mpg123.h>
#include<cstdint>

/*
 * Decoder is a class responsible for decoding a mp3/ogg/wav-File
 * and generating a JRListItem, so it can be displayed with
 * a gui framework
 */

class Decoder
{
  typedef std::vector<std::vector<float>> SampleBuffer;
  private:
  enum FileFormat
  {
    MP3,
    WAV,
    OGG,
    UNKNOWN
  };
  enum class Endianness
  {
    LittleEndian,
    BigEndian
  };

  public:
  /* Constructor */
  Decoder();
  /* Destructor */
  ~Decoder();

  //==================
  /* Misc-Functions */
  //==================

  bool load(const std::string &fileToRead);
  void downmix(const int &samplesPerChunk=1);
  bool writeToFile(const std::string &fileToWrite);
  bool writeExtToFile(const std::string &fileToWrite);

  //=================
  /* Getter/Setter */
  //=================
  int16_t getAudioF();
  void setAudioF(const FileFormat &val);
  uint32_t getSampleRate();
  void setSampleRate(const uint32_t &val);
  int16_t getBitDepth();
  void setBitDepth(const int16_t &val);
  int16_t getNrOfChannels();
  void setNrOfChannels(const int16_t &val);
  uint32_t getNrOfSamples();
  void setNrOfSamples(const uint32_t &val);
  int getChunkSize();
  void setChunkSize(const int &val);
  void setFilename(const int &val);
  int getLength();
  std::string getFilename();
  const SampleBuffer& getSampleBuffer() const;
  const std::vector<float>& getDSampleBuffer() const;

  private:
  SampleBuffer samples;
  std::vector<float> vec_downmixed_samples;
  FileFormat audiof;
  std::string filename;
  uint32_t samplingrate=0;
  int16_t bitspersample=0;
  int16_t nr_of_channels=0;
  uint32_t nr_of_samples=0;
  int chunk_size=0;

  std::string findFilename(const std::string &file);
  FileFormat checkFileFormat(const std::string &file);
  float singleByteToSample(const uint8_t &data);
  float sixteenBitIntToSample(const uint16_t &data);
  int16_t twoBytesToInt(const std::vector<uint8_t> &fileData,
      int32_t offset, Endianness endianness = Endianness::LittleEndian);
  int32_t fourBytesToInt(const std::vector<uint8_t> &fileData,
      int32_t offset, Endianness endianness = Endianness::LittleEndian);
  int32_t getIndexOfString(const std::vector<uint8_t> &fileData,
      const std::string &string);
  void clearSampleBuffer();
  void clearDSampleBuffer();
  bool handleMp3File(const std::string &file);
  bool decodeWaveFile(const std::vector<uint8_t> &fileData);
  bool decodeOggFile(const std::vector<uint8_t> &fileData);
};
#endif
