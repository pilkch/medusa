#ifndef MEDUSA_GTKMMPREFERENCESDIALOG_H
#define MEDUSA_GTKMMPREFERENCESDIALOG_H

// gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/util/string.h>

class cGtkmmPreferencesDialog : public Gtk::Dialog
{
public:
  explicit cGtkmmPreferencesDialog(Gtk::Window& parent);
  virtual ~cGtkmmPreferencesDialog() {}

  bool Run();

private:
  void OnLastfmEnabledToggled();

  Gtk::Alignment m_Alignment;

  // Controls
  // Playback
  Gtk::Frame groupPlayback;
  Gtk::VBox boxPlayback;;
  Gtk::CheckButton playbackRepeat;
  Gtk::CheckButton playbackNotifyOnSongChange;

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

  Gtk::Separator separator;
};

#endif // !MEDUSA_GTKMMPREFERENCESDIALOG_H
