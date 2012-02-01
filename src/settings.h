#ifndef MEDUSA_SETTINGS_H
#define MEDUSA_SETTINGS_H

#include <spitfire/storage/document.h>

// ** cSettings

class cSettings
{
public:
  void Load();
  void Save();

  uint32_t GetVolume0To100() const;
  void SetVolume0To100(uint32_t uiVolume);

  bool IsModeRepeat() const;
  void SetModeRepeat(bool bRepeat);

private:
  template <class T>
  T GetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& valueDefault) const;
  template <class T>
  void SetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& value);

  spitfire::document::cDocument document;
};

#endif // MEDUSA_SETTINGS_H
