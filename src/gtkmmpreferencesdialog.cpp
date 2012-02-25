// Medusa headers
#include "gtkmmpreferencesdialog.h"

cGtkmmPreferencesDialog::cGtkmmPreferencesDialog(cSettings& _settings, Gtk::Window& parent) :
  Gtk::Dialog("PreferencesDialog", parent, true),
  settings(_settings),
  groupPlayback("Playback"),
  playbackRepeat("Repeat"),
  playbackNotifyOnSongChange("Notify on song change"),
  groupLastfm("Last.fm"),
  lastfmEnabled("Use Last.fm"),
  lastfmTable(2, 2),
  lastfmUserNameDescription("User:"),
  lastfmPasswordDescription("Password:"),
  lastfmNewToLastfmDescription("New to Last.fm?"),
  lastfmSignUpForAnAccount("http://www.last.fm/join", "Sign up for an account"),
  pOkButton(nullptr)
{
  set_resizable();

  signal_response().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnResponse));

  Gtk::Box* pBox = get_vbox();

  pBox->set_border_width(10);

  playbackRepeat.set_active(settings.IsRepeat());
  playbackNotifyOnSongChange.set_active(settings.IsNotifyOnSongChange());

  // Playback
  pBox->pack_start(groupPlayback, Gtk::PACK_SHRINK);
  groupPlayback.add(boxPlayback);
  boxPlayback.pack_start(playbackRepeat, Gtk::PACK_SHRINK);
  boxPlayback.pack_start(playbackNotifyOnSongChange, Gtk::PACK_SHRINK);

  lastfmEnabled.signal_toggled().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));

  lastfmEnabled.set_active(settings.IsLastFMEnabled());
  lastfmUserName.set_text(spitfire::string::ToUTF8(settings.GetLastFMUserName()).c_str());
  lastfmPassword.set_text(spitfire::string::ToUTF8(settings.GetLastFMPassword()).c_str());

  const bool bEnabled = false;
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmUserName.signal_changed().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));
  lastfmUserName.set_activates_default(true);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);
  lastfmPassword.signal_changed().connect(sigc::mem_fun(*this, &cGtkmmPreferencesDialog::OnEnableControls));
  lastfmPassword.set_activates_default(true);

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
  boxLastfm.pack_start(boxLastfmSignup, Gtk::PACK_SHRINK);
  boxLastfmSignup.pack_start(lastfmNewToLastfmDescription, Gtk::PACK_SHRINK);
  boxLastfmSignup.pack_start(lastfmSignUpForAnAccount, Gtk::PACK_SHRINK);

  // Add separator
  pBox->pack_start(separator, Gtk::PACK_SHRINK);

  // Add standard buttons
  add_button(Gtk::Stock::CANCEL, Gtk::ResponseType::RESPONSE_CANCEL);
  pOkButton = add_button(Gtk::Stock::OK, Gtk::ResponseType::RESPONSE_OK);
  set_default_response(Gtk::ResponseType::RESPONSE_OK);

  // Make sure that our controls are in the correct state
  OnEnableControls();

  show_all_children();
}

void cGtkmmPreferencesDialog::OnResponse(int response_id)
{
  if (response_id == Gtk::ResponseType::RESPONSE_OK) {
    settings.SetRepeat(playbackRepeat.get_active());
    settings.SetNotifyOnSongChange(playbackNotifyOnSongChange.get_active());

    settings.SetLastFMEnabled(lastfmEnabled.get_active());
    settings.SetLastFMUserName(spitfire::string::ToString_t(lastfmUserName.get_text()));
    settings.SetLastFMPassword(spitfire::string::ToString_t(lastfmPassword.get_text()));
  }
}

void cGtkmmPreferencesDialog::OnEnableControls()
{
  const bool bEnabled = lastfmEnabled.get_active();
  lastfmUserNameDescription.set_sensitive(bEnabled);
  lastfmUserName.set_sensitive(bEnabled);
  lastfmPasswordDescription.set_sensitive(bEnabled);
  lastfmPassword.set_sensitive(bEnabled);

  if (pOkButton != nullptr) pOkButton->set_sensitive(!bEnabled || (bEnabled && !lastfmUserName.get_text().empty() && !lastfmPassword.get_text().empty()));
}

bool cGtkmmPreferencesDialog::Run()
{
  return (run() == Gtk::ResponseType::RESPONSE_OK);
}
