function buttonWifiUserInput() {
    const button = document.getElementById("button-save");
    button.focus();
    submitUserInput("WiFi-form", button);
}

function buttonTimeUserInput() {
    const form = document.getElementById("time-form");
    if (!form.reportValidity()) return;

    const button = document.getElementById("button-save-2");
    submitUserInput("time-form", button);
}

function buttonSetTimeAuto() {
    renderNewDiv("PAGE-home");
}

function buttonSetTimeManually() {
    renderNewDiv("PAGE-setTime");
}

function buttonColorPicker() {
    startTimeout();
    fetch("/color/start", {
        method: 'GET'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })
        .then(() => {
            resetTimeout();
            renderNewWindow("color-window");
        })
        .catch(() => {
            error();
        });
}

function buttonPlaySnake() {
    startTimeout();
    fetch("/snake/start", {
        method: 'GET'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })
        .then(() => {
            resetTimeout();
            renderNewWindow("snake-window");
            prevSnakeButton = "";
        })
        .catch(() => {
            error();
        });
}

function buttonExit() {
    renderNewDiv("PAGE-exit");
}

function buttonRestart(button) {
    button.classList.add('button--loading');
    disableAllButtons(true);
    startTimeout();

    fetch('/restart', {
        method: 'GET'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })

        // Render "Device is now restarting" page
        // you have to atleast see the animations right? (settimeout)
        .then(() => {
            resetTimeout();
            setTimeout(() => {
                renderNewDiv("PAGE-restart");
                disableAllButtons(true, true);
                document.getElementById('icon-saved').style.background = "var(--reboot-color)";
                button.classList.remove('button--loading');
            }, 1100);
        })
        .catch(() => {
            error();
        });
}

function buttonSnakeDirection(button) {
    if (button != prevSnakeButton) {
        fetch("snake/" + button, {
            method: 'POST'
        });

        prevSnakeButton = button;
    }
}

function buttonSnakeBack() {
    startTimeout();

    fetch("/snake/end", {
        method: 'GET'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })
        .then(() => {
            resetTimeout();
            renderNewWindow("app-window");
        })
        .catch(() => {
            error();
        });
}

function buttonColorSave(button) {
    button.classList.add('button--loading');
    disableAllButtons(true);
    startTimeout();

    fetch('/color/end', {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: currentHexColor,
        cache: 'no-store'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })

        // You have to atleast see the animations right? (setTimeout)
        .then(() => {
            resetTimeout();
            setTimeout(() => {
                renderNewWindow("app-window");
                disableAllButtons(false);
                button.classList.remove('button--loading');
            }, 1100);
        })
        .catch(() => {
            button.classList.remove('button--loading');
            error();
        });
}

function buttonColorBack() {
    startTimeout();

    fetch("/", {
        method: 'GET'
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })
        .then(() => {
            resetTimeout();
            renderNewWindow("app-window");
        })
        .catch(() => {
            error();
        });
}
