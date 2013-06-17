if (typeof(EventSource) !== "undefined") {
  var source = new EventSource("/updates");

  //source.onopen = function(e) {
  //  $("#test").html("Connected to server. Waiting for data...");
  //}

  source.onmessage = function(event) {
    document.getElementById("tracks").innerHTML += "Data: " + event.data + "<br/>";
  };

  // Listen for specific messages
  source.addEventListener("OnActionPlayTrack", function(event) {
    //if (event.origin != 'http://example.com') {
      var track = JSON.parse(event.data);

      var tr = document.createElement("tr");

      tr.setAttribute("class", "table_border");

      // Set the content of the row
      tr.innerHTML =
       "    <th class=\"table_border\">" + track.sArtist + "</th> \
            <th class=\"table_border\">" + track.sTitle + "</th> \
            <th class=\"table_border\">" + track.sDurationMS + "</th> \
            <th class=\"table_border\"> \
              <a href=\"download/" + track.sFilePath + "\"><img src=\"images/file_save.png\" alt=\"Save File\" width=\"16\" height=\"16\"/></a> \
            </th> \
            <th class=\"table_border\"> \
              <a href=\"javascript:;\" onclick=\"OnActionDeleteTrack(" + track.id + ")\" class=\"image_button\"> \
                <img src=\"images/file_trash.png\" width=\"16\" height=\"16\" alt=\"Delete File\"/> \
              </a> \
            </th>"
      ;

      // Add the row to the tracks table
      var tracklist_header = document.getElementById("tracklist_header");
      var parent = tracklist_header.parentNode;
      var next = tracklist_header.nextSibling;

      // Insert after the header
      parent.insertBefore(tr, next);
    //}
  }, false);

  // Handle errors
  source.onerror = function(event) {
    //alert("EventSource error");
    if (event.readyState == EventSource.CONNECTING) {
      // Reconnecting
    } else if (event.readyState == EventSource.CLOSED) {
      // Connection was closed.
    }
  };
}
