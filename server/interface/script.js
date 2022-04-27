
function updateStatus() {
	if (right && !left) {
		document.getElementById("status").innerText = "status: turning right";
	} else if (left && !right) {
		document.getElementById("status").innerText = "status: turning left";
	} else if (up && !down) {
		document.getElementById("status").innerText = "status: moving forward";
	} else if (down && !up) {
		document.getElementById("status").innerText = "status: moving backwards";
	} else {
		document.getElementById("status").innerText = "status: stationary";
	}
}

function updateVideo() {
	document.getElementById("video").src = "../camera/grover_image.jpeg?" + (new Date()).getTime();
}

setInterval(updateStatus, 50);
setInterval(updateVideo, 50);