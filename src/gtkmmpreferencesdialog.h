#ifndef MEDUSA_GTKMMPREFERENCESDIALOG_H
#define MEDUSA_GTKMMPREFERENCESDIALOG_H

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "medusa.h"
#include "settings.h"

namespace medusa
{
class cGtkmmPreferencesDialog : public Gtk::Dialog
{
public:
  cGtkmmPreferencesDialog(cSettings& settings, Gtk::Window& parent);
  virtual ~cGtkmmPreferencesDialog() {}

  bool Run();

private:
  void OnResponse(int response_id);
  void OnEnableControls();

  cSettings& settings;

  Gtk::Alignment m_Alignment;

  // Controls
  // Playback
  Gtk::Frame groupPlayback;
  Gtk::VBox boxPlayback;;
  Gtk::CheckButton playbackRepeat;
  Gtk::CheckButton playbackNotifyOnSongChange;
  Gtk::CheckButton playbackNextSongOnMoveToTrash;

  // Last.fm
  Gtk::Frame groupLastfm;
  Gtk::VBox boxLastfm;
  Gtk::CheckButton lastfmEnabled;
  Gtk::Table lastfmTable;
  Gtk::Label lastfmUserNameDescription;
  Gtk::Entry lastfmUserName;
  Gtk::Label lastfmPasswordDescription;
  Gtk::Entry lastfmPassword;
  Gtk::HBox boxLastfmSignup;
  Gtk::Label lastfmNewToLastfmDescription;
  Gtk::LinkButton lastfmSignUpForAnAccount;

  // Web Server
  Gtk::Frame groupWebServer;
  Gtk::VBox boxWebServer;
  Gtk::CheckButton webServerEnabled;
  Gtk::LinkButton webServerLink;

  Gtk::Separator separator;

  Gtk::Button* pOkButton;
};
}

#endif // !MEDUSA_GTKMMPREFERENCESDIALOG_H
