let prevDiv = "PAGE-home";
let prevWindow = "app-window";
let isSessionStorage = isSessionStorageSupported();
let isSessionSaved = false;
let timeout;
let prevSnakeButton;

const pickerCanvas = document.getElementById('picker-canvas');
const pickerCtx = pickerCanvas.getContext('2d');
const hueCanvas = document.getElementById('hue-canvas');
const hueCtx = hueCanvas.getContext('2d');
const outputRGB = document.getElementById('output-rgb');
const outputHex = document.getElementById('output-hex');

const colorHexInterval = 5; // ms

let currentHexColor = '#000000';
let hue = 0;
let sat = 100;
let val = 100;
let pickerSelectorX = 0;
let pickerSelectorY = 0;
let hueSelectorX = 0;
let colorHexLastSent = 0;
let isStartup = true;


function isSessionStorageSupported() {
    try {
        sessionStorage.setItem("test", "test_value");
        sessionStorage.removeItem("test");
        return true;
    } catch {
        return false;
    }
}

function displaySavedState() {
    if (!isSessionStorage) return;

    const state = sessionStorage.getItem("saved_state");
    if (state && state === "true") {
        setTimeout(() => {
            document.getElementById('icon-saved').classList.add('show');
        }, 200);

        isSessionSaved = true;
    }
}

function updateSavedState(isSaved) {
    if (!isSessionStorage) return;

    // Write to sessionStorage and show/hide icon
    if (isSaved) {
        isSessionSaved = true;
        sessionStorage.setItem("saved_state", "true");
        document.getElementById('icon-saved').classList.add('show');
    } else {
        isSessionSaved = false;
        sessionStorage.setItem("saved_state", "false");
        document.getElementById('icon-saved').classList.remove('show');
    }
}

function isBrowserFirefox() {
    const firefox = navigator.userAgent.toLowerCase().includes('firefox');

    if (firefox) {
        setTimeout(() => {
            alert("Firefox has limited support! \nManually setting the time wont work."); // ;)
        }, 100);
    }
}

function startTimeout() {
    resetTimeout(timeout);

    timeout = setTimeout(() => {
        error();
    }, 6000);
}

function resetTimeout() {
    clearTimeout(timeout);
}

function renderNewDiv(newDiv) {
    if (!newDiv || newDiv === prevDiv) return;

    document.getElementById(prevDiv).style.display = 'none';
    document.getElementById(newDiv).style.display = 'block';

    if (newDiv === "PAGE-home") {
        toggleTimebutton(newDiv);

    } else if (newDiv === "PAGE-setTime") {
        toggleTimebutton(newDiv);
        isBrowserFirefox();

        // Change "restart" btn color dependent on saved state
    } else if (newDiv === "PAGE-exit") {
        const button = document.getElementById("button-exit");

        if (isSessionSaved) {
            button.style.backgroundColor = "var(green-btn-color)";
        } else {
            button.style.backgroundColor = "var(--save-btn-color)";
        }
    }

    resetDiv(prevDiv);
    prevDiv = newDiv;
}

function renderNewWindow(newWindow) {
    // Last min fix to display snake game controls and hide app window
    if (newWindow == "snake-window") {
        document.getElementById("app-window").style.display = 'none';
        document.getElementById("snake-window").style.display = 'block';
        document.getElementById("color-window").style.display = 'none';

    } else if (newWindow == "color-window") {
        document.getElementById("app-window").style.display = 'none';
        document.getElementById("snake-window").style.display = 'none';
        document.getElementById("color-window").style.display = 'block';

    } else if (newWindow == "app-window") {
        document.getElementById("app-window").style.display = 'block';
        document.getElementById("snake-window").style.display = 'none';
        document.getElementById("color-window").style.display = 'none';
        renderNewDiv("PAGE-home");
    }

    prevWindow = newWindow;
}

function toggleTimebutton(new_div) {
    const button = document.getElementById("flex-button");

    if (new_div === "PAGE-home") {
        button.textContent = "Set Time Manually";
        button.onclick = buttonSetTimeManually;
    } else {
        button.textContent = "Set Time Automatically";
        button.onclick = buttonSetTimeAuto;
    }
}

function disableAllButtons(disabled, buttonStyle = false) {
    const buttons = document.querySelectorAll('button');

    for (const btn of buttons) {
        btn.disabled = disabled;
        btn.style.cursor = (disabled) ? "initial" : "pointer";

        if (buttonStyle) {
            btn.style.backgroundColor = "var(--reboot-color)";
        }
    }
}

function WifiFormColor(inputField) {
    inputField.style.transition = "border-color 0.4s ease-in";

    if (inputField && inputField.value.trim() !== "") {
        inputField.style.borderColor = "var(--green-btn-color)";
    } else {
        inputField.style.borderColor = "var(--accent-grey)";
    }

    // Save button color
    const ssid = document.getElementById('ssid').value.trim() !== '';
    const password = document.getElementById('password').value.trim() !== '';
    const saveButton = document.getElementById('button-save');
    saveButton.style.transition = "background 0.4s ease-in";

    if (ssid && password) {
        saveButton.style.background = "var(--green-btn-color)";
    } else {
        saveButton.style.background = "var(--save-btn-color)";
    }
}

function timeFormColor(inputField) {
    const saveButton = document.getElementById("button-save-2");
    const border = document.getElementById("time-container");
    const text = document.getElementById("time-placeholder-text");

    // Transitions
    saveButton.style.transition = "background 0.4s ease-in";
    border.style.transition = "border-color 0.4s ease-in";

    // Change the time-form appearance
    if (inputField.value.trim() !== "") {
        text.textContent = inputField.value;
        text.style.color = "var(--white)";
        border.style.borderColor = "var(--green-btn-color)";
        saveButton.style.background = "var(--green-btn-color)";
    }
}

function resetDiv(div) {
    switch (div) {
        case "PAGE-home":
            document.getElementById("WiFi-form").reset();
            document.getElementById('button-save').style.background = "var(--save-btn-color)";
            document.getElementById("ssid").style.borderColor = "var(--accent-grey)";
            document.getElementById("password").style.borderColor = "var(--accent-grey)";
            break;

        case "PAGE-setTime":
            const text = document.getElementById("time-placeholder-text");
            text.style.color = "var(--accent-grey)";
            text.textContent = "00:00";
            document.getElementById("time-form").reset();
            document.getElementById("time-container").style.borderColor = "var(--accent-grey)";
            document.getElementById("button-save-2").style.background = "var(--save-btn-color)";
            break;

        default:
            break;
    }
}

function submitUserInput(form, saveButton) {
    saveButton.classList.add('button--loading');
    disableAllButtons(true);

    // WiFi or time form?
    let data;
    if (form === "WiFi-form") {
        data = {
            ssid: document.getElementById("ssid").value,
            password: document.getElementById("password").value,
        };
    } else {
        data = {
            timeManual: document.getElementById("time-input").value,
        };
    }

    submitData(form, data, saveButton);
}

function submitData(targetForm, formData, saveButton) {
    startTimeout();

    fetch("/submit/" + targetForm, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            data: formData,
        })
    })
        .then(response => {
            if (!response.ok) { throw new Error(); }
        })

        // Success! Update UI and enable buttons
        // you have to atleast see the animations right? (settimeout)
        .then(() => {
            resetTimeout();
            isSessionSaved = true;
            setTimeout(() => {
                disableAllButtons(false);
                renderNewDiv("PAGE-saved");
                updateSavedState(true);
                saveButton.classList.remove('button--loading');
            }, 1500);
        })
        .catch(error => {
            error();
        });
}

function error() {
    if (prevWindow == "snake-window") {
        snakeError();
    } else if (prevWindow == "color-window") {
        colorError();
    } else {
        renderNewDiv("PAGE-error");
        updateSavedState(false);
        disableAllButtons(true);
    }
    /* END */
}

function snakeError() {
    document.getElementById("snake-text-h2").textContent = ("Uh-oh! Try refresh or restarting");
    document.getElementById("button-back").classList.add("button-error");
    document.getElementById("button-back").textContent = ("Error");
    updateSavedState(false);
    disableAllButtons(true);
    /* END */
}

function colorError() {
    const saveButton = document.getElementById("color-button-save");
    const backButton = document.getElementById("color-button-back");

    document.getElementById("color-text-h2").textContent = ("Uh-oh! Try refresh or restarting");
    saveButton.classList.add("button-error");
    saveButton.textContent = ("Error");
    saveButton.style.width = "100%";
    backButton.style.display = "none";

    updateSavedState(false);
    disableAllButtons(true);
    /* END */
}

pickerCanvas.addEventListener('mousedown', handlePickerInteraction);
pickerCanvas.addEventListener('touchstart', handlePickerInteraction, { passive: false });
hueCanvas.addEventListener('mousedown', handleHueInteraction);
hueCanvas.addEventListener('touchstart', handleHueInteraction, { passive: false });

displaySavedState();
drawHueSlider();
drawPicker();
updateOutput();
