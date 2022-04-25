document.addEventListener("keydown", (e) => {
    switch(e.key) {
        case "ArrowUp":
            if(document.getElementById("up").innerText == "0") {
                document.getElementById("up").innerText = "1";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("up=1");
            }
            break;
        case "ArrowDown":
            if(document.getElementById("down").innerText == "0") {
                document.getElementById("down").innerText = "1";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("down=1");
            }
            break;
        case "ArrowLeft":
            if(document.getElementById("left").innerText == "0") {
                document.getElementById("left").innerText = "1";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("left=1");
            }
            break;
        case "ArrowRight":
            if(document.getElementById("right").innerText == "0") {
                document.getElementById("right").innerText = "1";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("right=1");
            }
            break;
    }
});
document.addEventListener("keyup", (e) => {
    switch(e.key) {
        case "ArrowUp":
            if(document.getElementById("up").innerText == "1") {
                document.getElementById("up").innerText = "0";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("up=0");
            }
            break;
        case "ArrowDown":
            if(document.getElementById("down").innerText == "1") {
                document.getElementById("down").innerText = "0";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("down=0");
            }
            break;
        case "ArrowLeft":
            if(document.getElementById("left").innerText == "1") {
                document.getElementById("left").innerText = "0";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("left=0");
            }
            break;
        case "ArrowRight":
            if(document.getElementById("right").innerText == "1") {
                document.getElementById("right").innerText = "0";
                let xhr = new XMLHttpRequest();
                xhr.open("POST", "https://608dev-2.net/sandbox/sc/team24/chassis/server.py");
                xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                xhr.send("right=0");
            }
            break;
    }
});