
document.addEventListener("DOMContentLoaded", function () {
  // Add an event listener for the button click
  var doorButton = document.getElementById("open-lights"); // Make sure to set the correct ID for your button
  doorButton.addEventListener("click", function () {
    // Make an asynchronous HTTP request to the server
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/openLights", true); // Update the endpoint to match your server routing
    xhr.onload = function () {
      if (xhr.status == 200) {
        $("#open-lights").removeClass().addClass("button1");
        $("#close-lights").removeClass().addClass("button2");
        $("#auto-lights").removeClass().addClass("button2");
        console.log("lights openned successfully");
        // You can add additional actions here if needed
      } else {
        console.error("Failed to open lights");
      }
    };
    xhr.send();
  });
});

document.addEventListener("DOMContentLoaded", function () {
  // Add an event listener for the button click
  var doorButton = document.getElementById("close-lights"); // Make sure to set the correct ID for your button
  doorButton.addEventListener("click", function () {
    // Make an asynchronous HTTP request to the server
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/closeLights", true); // Update the endpoint to match your server routing
    xhr.onload = function () {
      if (xhr.status == 200) {
        $("#open-lights").removeClass().addClass("button2");
        $("#close-lights").removeClass().addClass("button1");
        $("#auto-lights").removeClass().addClass("button2");
        console.log("lights closed successfully");
        // You can add additional actions here if needed
      } else {
        console.error("Failed to close lights");
      }
    };
    xhr.send();
  });
});

document.addEventListener("DOMContentLoaded", function () {
  // Add an event listener for the button click
  var doorButton = document.getElementById("auto-lights"); // Make sure to set the correct ID for your button
  doorButton.addEventListener("click", function () {
    // Make an asynchronous HTTP request to the server
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/autoLights", true); // Update the endpoint to match your server routing
    xhr.onload = function () {
      if (xhr.status == 200) {
        $("#open-lights").removeClass().addClass("button2");
        $("#close-lights").removeClass().addClass("button2");
        $("#auto-lights").removeClass().addClass("button1");
        console.log("automatic lights activated successfully");
        // You can add additional actions here if needed
      } else {
        console.error("Failed to activate automatic lights");
      }
    };
    xhr.send();
  });
});

document.addEventListener("DOMContentLoaded", function () {
  // Add an event listener for the button click
  var doorButton = document.getElementById("open-entrance-door"); // Make sure to set the correct ID for your button
  doorButton.addEventListener("click", function () {
    // Make an asynchronous HTTP request to the server
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/openEntranceDoor", true); // Update the endpoint to match your server routing
    xhr.onload = function () {
      if (xhr.status == 200) {
        console.log("entrance door openned successfully");
        // You can add additional actions here if needed
      } else {
        console.error("Failed to open entrance door");
      }
    };
    xhr.send();
  });
});

document.addEventListener("DOMContentLoaded", function () {
  // Add an event listener for the button click
  var doorButton = document.getElementById("open-exit-door"); // Make sure to set the correct ID for your button
  doorButton.addEventListener("click", function () {
    // Make an asynchronous HTTP request to the server
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/openExitDoor", true); // Update the endpoint to match your server routing
    xhr.onload = function () {
      if (xhr.status == 200) {
        console.log("exit door openned successfully");
        // You can add additional actions here if needed
      } else {
        console.error("Failed to open exit door");
      }
    };
    xhr.send();
  });
});

$(document).ready(function(){
  function updateStatus() {
    $.ajax({
      url: '/getStatus',
      type: 'GET',
      success: function(data) {
        updateData(data);
      },
      error: function(error) {
        console.error('Error status:', error);
      }
    });
  }

  function updateData(data) {
    var noAv = data.noAv || 0;
    var nearestObj = data.nearestObj || 0;
    var time = data.time || 'Day';
    var lightStatus = data.lightStatus || 'Off';
    $('#day-night').text(time);
    $('#available-parkings').text('Available Parkings: '+noAv);
    $('#nearest-object').text('Nearest Object: '+ nearestObj + 'cm');

    if(lightStatus == 'on'){
      $("#open-lights").removeClass().addClass("button1");
      $("#close-lights").removeClass().addClass("button2");
      $("#auto-lights").removeClass().addClass("button2");
    }
    else if(lightStatus == 'off'){
      $("#open-lights").removeClass().addClass("button2");
      $("#close-lights").removeClass().addClass("button1");
      $("#auto-lights").removeClass().addClass("button2");
    }
    else if(lightStatus == 'auto'){
      $("#open-lights").removeClass().addClass("button2");
      $("#close-lights").removeClass().addClass("button2");
      $("#auto-lights").removeClass().addClass("button1");
    }
  }

  setInterval(updateStatus, 1000); 
});
