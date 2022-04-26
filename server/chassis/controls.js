let up = false;
let down = false;
let left = false;
let right = false;

function sendKeyChange(key, state) {
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/GROVER/server/chassis/server.py");
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.send(key + "=" + state);
}

document.addEventListener("keydown", (e) => {
    switch(e.key) {
        case "ArrowUp":
            if (!up) { sendKeyChange("up", "1"); }
            up = true;
            break;
        case "ArrowDown":
            if (!down) { sendKeyChange("down", "1"); }
            down = true;
            break;
        case "ArrowLeft":
            if (!left) { sendKeyChange("left", "1"); }
            left = true;
            break;
        case "ArrowRight":
            if (!right) { sendKeyChange("right", "1"); }
            right = true;
            break;
    }
});

document.addEventListener("keyup", (e) => {
    switch(e.key) {
        case "ArrowUp":
            if (up) { sendKeyChange("up", "0"); }
            up = false;
            break;
        case "ArrowDown":
            if (down) { sendKeyChange("down", "0"); }
            down = false;
            break;
        case "ArrowLeft":
            if (left) { sendKeyChange("left", "0"); }
            left = false;
            break;
        case "ArrowRight":
            if (right) { sendKeyChange("right", "0"); }
            right = false;
            break;
    }
});