#ifndef MEDUSA_SETTINGS_H
#define MEDUSA_SETTINGS_H

// Spitfire headers
#include <spitfire/storage/document.h>

// ** cSettings

class cSettings
{
public:
  void Load();
  void Save();

  bool IsShowMainWindow() const;
  void SetShowMainWindow(bool bShowMainWindow);

  uint32_t GetVolume0To100() const;
  void SetVolume0To100(uint32_t uiVolume);

  bool IsRepeat() const;
  void SetRepeat(bool bRepeat);

private:
  template <class T>
  T GetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& valueDefault) const;
  template <class T>
  void SetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& value);

  spitfire::document::cDocument document;
};

#endif // MEDUSA_SETTINGS_H
