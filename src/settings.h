#ifndef MEDUSA_SETTINGS_H
#define MEDUSA_SETTINGS_H

// Spitfire headers
#include <spitfire/storage/document.h>

namespace medusa
{
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

  bool IsNotifyOnSongChange() const;
  void SetNotifyOnSongChange(bool bNotifyOnSongChange);

  bool IsLastFMEnabled() const;
  void SetLastFMEnabled(bool bEnabled);

  spitfire::string_t GetLastFMUserName() const;
  void SetLastFMUserName(const spitfire::string_t& sUserName);

  spitfire::string_t GetLastFMPassword() const;
  void SetLastFMPassword(const spitfire::string_t& sPassword);

private:
  template <class T>
  T GetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& valueDefault) const;
  template <class T>
  void SetXMLValue(const spitfire::string_t& sSection, const spitfire::string_t& sItem, const spitfire::string_t& sAttribute, const T& value);

  spitfire::document::cDocument document;
};
}

#endif // MEDUSA_SETTINGS_H
