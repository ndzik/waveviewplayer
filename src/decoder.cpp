#include "decoder.h"
/*
 * Decoder is a class responsible for decoding a mp3/ogg/wav-File
 * and generating a JRListItem, so it can be displayed with
 * a gui framework
 */

/* Constructor */
Decoder::Decoder()
{

}
/* Destructor */
Decoder::~Decoder()
{

}

//==================
/* Misc-Functions */
//==================

bool Decoder::load(const std::string &fileToRead)
{
  std::ifstream file(fileToRead, std::ios::binary);

  //check if file exists
  if(!file.good())
  {
    std::cout << "ERROR: File coudln't be loaded" << std::endl;
    std::cout << fileToRead << std::endl;
    return false;
  }

  file.unsetf(std::ios::skipws);
  std::istream_iterator<uint8_t> begin(file), end;
  std::vector<uint8_t> fileData(begin, end);

  //get fileformat to decode properly
  FileFormat fileFormat = checkFileFormat(fileToRead);

  switch(fileFormat)
  {
    case MP3:
      return handleMp3File(fileToRead);
    case WAV:
      return decodeWaveFile(fileData);
    case OGG:
      return decodeOggFile(fileData);
    case UNKNOWN:
      return false;
  }

  std::cout << "Could not determine audiofileformat" << std::endl;
  return false;
}

std::string Decoder::findFilename(const std::string &file)
{
  std::string filename;
#ifdef __linux__
  filename = file.substr(file.find_last_of('/')+1, file.find_last_of('.'));
#elif _WIN32
  filename = file.substr(file.find_last_of('\\')+1, file.find_last_of('.'));
#endif
  return filename;
}

Decoder::FileFormat Decoder::checkFileFormat(const std::string &file)
{
  if(file.find_last_of(".") != std::string::npos)
  {
    std::string ext = file.substr(file.find_last_of(".")+1);
    std::string name = file.substr(0, file.find_last_of(".")-1);
    if(ext == "mp3")
    {
      filename = findFilename(file);
      return MP3;
    }
    if(ext == "wav")
    {
      filename = findFilename(file);
      return WAV;
    }
    if(ext == "ogg")
    {
      filename = findFilename(file);
      return OGG;
    }
  }
  return UNKNOWN;
}

/*
 * Decodes a .wav file and writes its PCM-Data to sampleBuffer
 * for further processing
 * @param fileData - vector that containa fileData of to decoding file
 * @return bool - true for success, false for failure
 */
bool Decoder::decodeWaveFile(const std::vector<uint8_t> &fileData)
{
  std::cout << "decoding wave file" << std::endl;
  // GET HEADER
  std::string headerChunkID(fileData.begin(), fileData.begin()+4);
  // Filesize in int32_t ==> corresponds to 4 uint8_t (bytes)
  std::string format(fileData.begin()+8, fileData.begin()+12); //normally 8&12 (whitespace?)

  //try and find starting points of fmt and data chunks
  int32_t indexOfDataChunk = getIndexOfString(fileData, "data");
  int32_t indexOfFormatChunk = getIndexOfString(fileData, "fmt");

  //if data or format chunks couldnt be found abort
  if(indexOfDataChunk == -1 || indexOfFormatChunk == -1
      || headerChunkID != "RIFF"|| format != "WAVE")
  {
    std::cout << "File couldnt be parsed" << std::endl;
    return false;
  }

  //Format Chunk
  int32_t f = indexOfFormatChunk;
  std::string formatChunkID(fileData.begin()+f, fileData.begin()+f+4);
  int16_t audioFormat = twoBytesToInt(fileData, f+8);
  int16_t numChannels = twoBytesToInt(fileData, f+10);
  uint32_t sampleRate = fourBytesToInt(fileData, f+12);
  uint32_t byteRate = fourBytesToInt(fileData, f+16);
  int16_t numBytesPerBlock = twoBytesToInt(fileData, f+20);
  int16_t bitDepth = twoBytesToInt(fileData, f+22);
  int32_t numBytesPerSample = bitDepth/8;

  //Check if Data is PCM-Stream
  if(audioFormat != 1)
  {
    std::cout << "ERROR: This is a compressed .wav file" << std::endl;
    return false;
  }

  //Check number of Channels
  if(audioFormat < 1 || numChannels > 2)
  {
    std::cout << "ERROR: Neither mono or stereo wave-file" << std::endl;
    return false;
  }

  //Check if header data is consistent
  if(byteRate != (numChannels*sampleRate*bitDepth)/8 ||
      (numBytesPerBlock != (numChannels*numBytesPerSample)))
  {
    std::cout << "ERROR: Header data in this .wav file seems inconsistent" << std::endl;
    return false;
  }

  //Check if bitdepth is either 8, 16, or 24bit
  if(bitDepth != 8 && bitDepth != 16 && bitDepth != 24)
  {
    std::cout << "ERROR: Bit depth of file is not 8, 16 or 24 bits" << std::endl;
    return false;
  }

  //Data Chunk
  int32_t d = indexOfDataChunk;
  std::string dataChunkID(fileData.begin()+d, fileData.begin()+d+4);
  uint32_t dataChunkSize = fourBytesToInt(fileData, d+4);

  int32_t numSamples = dataChunkSize/(numChannels*bitDepth/8);
  int32_t samplesStartIndex = indexOfDataChunk+8;

  clearSampleBuffer();
  samples.resize(numChannels);

  for(int i=0; i<numSamples; i++)
  {
    for(int channel=0; channel < numChannels; channel++)
    {
      int sampleIndex = samplesStartIndex
                        + (numBytesPerBlock*i)+channel*numBytesPerSample;

      if(bitDepth==8)
      {
        float sample = singleByteToSample(fileData[sampleIndex]);
        samples[channel].push_back(sample);
      }
      else if(bitDepth==16)
      {
        int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
        float sample = sixteenBitIntToSample(sampleAsInt);
        samples[channel].push_back(sample);
      }
      else if(bitDepth==24)
      {
        int32_t sampleAsInt=0;
        sampleAsInt = (fileData[sampleIndex+2] << 16)
          | (fileData[sampleIndex+1] << 8) | fileData[sampleIndex];
        if(sampleAsInt & 0x800000)
          sampleAsInt = sampleAsInt | ~0xFFFFFF; //Sign extension to 32 bit float
        float sample = (float) sampleAsInt/8388608.f;
        samples[channel].push_back(sample);
      }
      else
      {
        std::cout << "Something went wrong with PCM-Data-Parsing" << std::endl;
      }
    }
  }
  setAudioF(WAV);
  setBitDepth(bitDepth);
  setNrOfSamples(numSamples);
  setNrOfChannels(numChannels);
  setSampleRate(sampleRate);
  return true;
}

void Decoder::clearDSampleBuffer()
{
  vec_downmixed_samples.clear();
}

void Decoder::clearSampleBuffer()
{
  for(int i=0; i<(int)samples.size(); i++)
    samples[i].clear();

  samples.clear();
}

int16_t Decoder::twoBytesToInt(const std::vector<uint8_t> &fileData,
    int32_t offset, Endianness endianness)
{
  int16_t result;

  if(endianness == Endianness::LittleEndian)
    result = (fileData[offset+1] << 8) | fileData[offset];
  else
    result = (fileData[offset] << 8) | fileData[offset+1];

  return result;
}

int32_t Decoder::fourBytesToInt(const std::vector<uint8_t> &fileData,
    int32_t offset, Endianness endianness)
{
  int32_t result;

  if(endianness == Endianness::LittleEndian)
  {
    result = (fileData[offset+3] << 24) | (fileData[offset+2] << 16)
             | (fileData[offset+1] << 8) | fileData[offset];
  }
  else
  {
    result = (fileData[offset] << 24) | (fileData[offset+1] << 16)
             | (fileData[offset+2] << 8) | fileData[offset+3];
  }

  return result;
}

int32_t Decoder::getIndexOfString(const std::vector<uint8_t> &fileData,
    const std::string &string)
{
  int index = -1;
  int stringLength = (int)string.length();

  for(int i=0; i< (int) fileData.size() - stringLength; i++)
  {
    std::string section(fileData.begin()+i, fileData.begin()+i+stringLength);

    if(section == string)
    {
      index = i;
      break;
    }
  }
  return index;
}

float Decoder::sixteenBitIntToSample(const uint16_t &data)
{
  int16_t tmp = data;
  return ((float) tmp)/32768.f;
}

float Decoder::singleByteToSample(const uint8_t &data)
{
  return ((float) data)/128.f;
}

/*
 * Decodes a .mp3 file and writes its PCM-Data to samplesBuffer
 * for further processing
 * @param fileData - vector that containa fileData of to decoding file
 * @return bool - true for success, false for failure
 */
bool Decoder::handleMp3File(const std::string &file)
{
  std::cout << "decoding mp3 file" << std::endl;

  mpg123_handle *mh;
  uint8_t *buffer;
  size_t buffer_size;
  size_t done;
  int err;
  int encoding;
  int numOfChannels;
  long sampleRate;

  mpg123_init();

  mh = mpg123_new(NULL, &err);

  /*
   * Determine MaxSize of a decoded Frames output
   * use to determine appropriate min buf size for dec. one frame
   */
  buffer_size = mpg123_outblock(mh);

  buffer = new uint8_t[buffer_size*sizeof(uint8_t)];

  /* Open file and get decoding format */
  mpg123_open(mh, file.c_str());
  mpg123_getformat(mh, &sampleRate, &numOfChannels, &encoding);
  int bitDepth = mpg123_encsize(encoding)*8; //Retuns size in bytes of one mono sample of named encoding

  /* decode */
  int totalBytes;
  int counter=0;

  clearSampleBuffer();
  samples.resize(numOfChannels);

  short* sSample16 = reinterpret_cast<short*>(buffer);

  for(totalBytes=0; mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK;)
  {
    /* Read buffer contents into our sampleBuffer */
    for(int i=0; i<(int)buffer_size/(bitDepth/8);)
    {
      for(int channel=0; channel<numOfChannels; channel++)
      {
        int16_t tmp = sSample16[i];
        float sample = tmp;
        samples[channel].push_back(sample/32768.f);
        counter++;
        i++;
      }
    }
  }
  setAudioF(MP3);
  setBitDepth(bitDepth);
  setNrOfSamples(counter/numOfChannels);
  setNrOfChannels(numOfChannels);
  setSampleRate(sampleRate);

  /* CleanUp */
  delete(buffer);
  mpg123_close(mh);
  mpg123_delete(mh);
  mpg123_exit();

  return true;
}

bool Decoder::decodeOggFile(const std::vector<uint8_t> &fileData)
{
  return false;
}

/*
 * Stereo+ will be downmixed to Mono
 *
 * @param samplesPerChunk: decides how many samples should be used per chunk
 *                         - The average will be taken
 *                         - Default=1
 */
void Decoder::downmix(const int &samplesPerChunk)
{
  float monoVal=0;
  float avg=0;
  float sum=0;
  setChunkSize(samplesPerChunk);
  clearDSampleBuffer();

  int quot = nr_of_samples/samplesPerChunk;
  quot = nr_of_samples%samplesPerChunk==0 ? quot : quot+1;

  /* Add avg of samplesPerChunk to vec_downmixed_samples-Vector */
  for(auto i=0; i < quot; i++)
  {
    /* sum of desired samplesPerChunk */
    for(auto j=0; j<samplesPerChunk; j++)
    {
      /* From Stereo+ -> Mono */
      for(auto k=0; k<nr_of_channels; k++)
      {
        monoVal = ((samples[k][j+i*samplesPerChunk]>=0 ?
                (monoVal + (samples[k][j+i*samplesPerChunk]))
              : (monoVal - (samples[k][j+i*samplesPerChunk]))));
      }
      monoVal = (monoVal/(float)nr_of_channels);
      sum = monoVal<0 ? sum-monoVal : sum+monoVal;
      monoVal = 0;
    }
    avg = (sum)/samplesPerChunk;
    vec_downmixed_samples.push_back(avg);
    sum = 0;
  }
}

bool Decoder::writeExtToFile(const std::string &fileToWrite)
{
  std::ofstream off(fileToWrite);
  for(int i=0; (unsigned int)i<nr_of_samples; i++)
  {
    off << "samples[" << 0 << "]:\t" << samples[1][i] << "\t";
    off << "samples[" << 1 << "]:\t" << samples[0][i] << std::endl;
  }
  off.close();
  return true;
}
/*
 * Writes downmixedsample-buffer to a file
 */
bool Decoder::writeToFile(const std::string &fileToWrite)
{
  std::ofstream off(fileToWrite);
  for(int i=0; (unsigned int)i<vec_downmixed_samples.size(); i++)
  {
    off << vec_downmixed_samples[i] << std::endl;
  }
  off.close();
  return true;
}

//===================
/* Getter & Setter */
//===================
int16_t Decoder::getAudioF()
{
  return audiof;
}
void Decoder::setAudioF(const FileFormat &val)
{
  this->audiof=val;
}
uint32_t Decoder::getSampleRate()
{
  return samplingrate;
}
void Decoder::setSampleRate(const uint32_t &val)
{
  this->samplingrate=val;
}
int16_t Decoder::getBitDepth()
{
  return bitspersample;
}
void Decoder::setBitDepth(const int16_t &val)
{
  this->bitspersample=val;
}
int16_t Decoder::getNrOfChannels()
{
  return nr_of_channels;
}
void Decoder::setNrOfChannels(const int16_t &val)
{
  this->nr_of_channels=val;
}
int Decoder::getChunkSize()
{
  return chunk_size;
}
void Decoder::setChunkSize(const int &val)
{
  this->chunk_size=val;
}
uint32_t Decoder::getNrOfSamples()
{
  return nr_of_samples;
}
void Decoder::setNrOfSamples(const uint32_t &val)
{
  this->nr_of_samples=val;
}
const Decoder::SampleBuffer& Decoder::getSampleBuffer() const
{
  return samples;
}
const std::vector<float>& Decoder::getDSampleBuffer() const
{
  return vec_downmixed_samples;
}
std::string Decoder::getFilename()
{
  return filename;
}
int Decoder::getLength()
{
  //Calculating length in ms
  int ms = nr_of_samples*1000/samplingrate;
  return ms;
}
