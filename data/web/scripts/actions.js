function PostAction(action)
{
  console.log("PostAction(\"%s\")", action);

  if (window.XMLHttpRequest) {
    // Send an AJAX request
    // Works in modern browsers
    var xmlhttp = new XMLHttpRequest();

    xmlhttp.onreadystatechange = function() {
      //if ((xmlhttp.readyState == 4) && (xmlhttp.status == 200)) {
      //  document.getElementById("myDiv").innerHTML = xmlhttp.responseText;
      //}
      /*if ((xmlhttp.readyState == 4) && (xmlhttp.status == 200)) {
        document.getElementById("myDiv").innerHTML = xmlhttp.responseText;
      }*/
    }
    xmlhttp.open("POST", "/action", true);
    xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlhttp.send("action=" + action);
  }
}

function OnActionPlaybackPrevious()
{
  PostAction("playback_previous");
}

function OnActionPlaybackPlay()
{
  PostAction("playback_play");
}

function OnActionPlaybackNext()
{
  PostAction("playback_next");
}

function OnActionVolumeMute()
{
  PostAction("volume_mute");
}

function OnActionVolumeFull()
{
  PostAction("volume_full");
}

function OnActionPlayTrack(id)
{
  PostAction("playback_play&track=" + id);
}

function OnActionDeleteTrack(id)
{
  // Send the request to the web server
  PostAction("file_trash&track=" + id);

  // Remove the track
  var entry = document.getElementById("entry_" + id);
  entry.parentNode.removeChild(entry);
}
