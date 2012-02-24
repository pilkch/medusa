// Medusa headers
#include "gtkmmpreferencesdialog.h"

cGtkmmPreferencesDialog::cGtkmmPreferencesDialog(Gtk::Window& parent) :
  Gtk::Dialog("PreferencesDialog", parent, true),
  groupPlayback("Playback"),
  playbackRepeat("Repeat"),
  playbackNotifyOnSongChange("Notify on song change"),
  groupLastfm("Last.fm"),
  lastfmEnabled("Use Last.fm"),
  lastfmTable(2, 2),
  lastfmUserNameDescription("User:"),
  lastfmPasswordDescription("Password:")
{
  set_resizable();

  Gtk::Box* pBox = get_vbox();

  pBox->set_border_width(10);

  // Playback
  pBox->pack_start(groupPlayback, Gtk::PACK_SHRINK);
  groupPlayback.add(boxPlayback);
  boxPlayback.pack_start(playbackRepeat, Gtk::PACK_SHRINK);
  boxPlayback.pack_start(playbackNotifyOnSongChange, Gtk::PACK_SHRINK);

  lastfmEnabled.signal_toggled().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnLastfmEnabledToggled));

  const bool bEnabled = false;
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);

  // Last.fm
  pBox->pack_start(groupLastfm, Gtk::PACK_SHRINK);
  groupLastfm.add(boxLastfm);
  boxLastfm.pack_start(lastfmEnabled, Gtk::PACK_SHRINK);
  boxLastfm.pack_start(lastfmTable);
  lastfmTable.attach(lastfmUserNameDescription, 0, 1, 0, 1, Gtk::SHRINK);
  lastfmTable.attach(lastfmUserName, 1, 2, 0, 1);
  lastfmTable.attach(lastfmPasswordDescription, 0, 1, 1, 2, Gtk::SHRINK);
  lastfmTable.attach(lastfmPassword, 1, 2, 1, 2);
  lastfmPassword.set_visibility(false);

  // Add separator
  pBox->pack_start(separator, Gtk::PACK_SHRINK);

  // Add standard buttons
  add_button(Gtk::Stock::CANCEL, Gtk::ResponseType::RESPONSE_CANCEL);
  add_button(Gtk::Stock::OK, Gtk::ResponseType::RESPONSE_OK);

  show_all_children();
}

void cGtkmmPreferencesDialog::OnLastfmEnabledToggled()
{
  const bool bEnabled = lastfmEnabled.get_active();
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);
}

bool cGtkmmPreferencesDialog::Run()
{
  return (run() == Gtk::ResponseType::RESPONSE_OK);
}
