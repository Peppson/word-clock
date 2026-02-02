function drawPicker() {
    pickerCtx.fillStyle = `hsl(${hue}, 100%, 50%)`;
    pickerCtx.fillRect(0, 0, pickerCanvas.width, pickerCanvas.height);

    const whiteGrad = pickerCtx.createLinearGradient(0, 0, pickerCanvas.width, 0);
    whiteGrad.addColorStop(0, 'rgba(255,255,255,1)');
    whiteGrad.addColorStop(1, 'rgba(255,255,255,0)');
    pickerCtx.fillStyle = whiteGrad;
    pickerCtx.fillRect(0, 0, pickerCanvas.width, pickerCanvas.height);

    const blackGrad = pickerCtx.createLinearGradient(0, 0, 0, pickerCanvas.height);
    blackGrad.addColorStop(0, 'rgba(0,0,0,0)');
    blackGrad.addColorStop(1, 'rgba(0,0,0,1)');
    pickerCtx.fillStyle = blackGrad;
    pickerCtx.fillRect(0, 0, pickerCanvas.width, pickerCanvas.height);

    drawPickerSelector();
}

function drawPickerSelector() {
    pickerCtx.lineWidth = 1;
    pickerCtx.strokeStyle = 'black';
    pickerCtx.beginPath();
    pickerCtx.arc(pickerSelectorX, pickerSelectorY, 5, 0, 2 * Math.PI);
    pickerCtx.stroke();

    pickerCtx.lineWidth = 2;
    pickerCtx.strokeStyle = 'white';
    pickerCtx.beginPath();
    pickerCtx.arc(pickerSelectorX, pickerSelectorY, 3, 0, 2 * Math.PI);
    pickerCtx.stroke();
}

function handlePickerInteraction(e) {
    e.preventDefault();

    function move(ev) {
        const clientX = ev.clientX ?? ev.touches[0].clientX;
        const clientY = ev.clientY ?? ev.touches[0].clientY;
        const rect = pickerCanvas.getBoundingClientRect();

        let x = clientX - rect.left;
        let y = clientY - rect.top;
        x = Math.min(rect.width, Math.max(0, x));
        y = Math.min(rect.height, Math.max(0, y));

        const scaleX = pickerCanvas.width / rect.width;
        const scaleY = pickerCanvas.height / rect.height;
        x = x * scaleX;
        y = y * scaleY;

        pickerSelectorX = x;
        pickerSelectorY = y;

        sat = (x / pickerCanvas.width) * 100;
        val = 100 - (y / pickerCanvas.height) * 100;

        drawPicker();
        updateOutput();
    }

    function up() {
        document.removeEventListener('mousemove', move);
        document.removeEventListener('mouseup', up);
        document.removeEventListener('touchmove', move);
        document.removeEventListener('touchend', up);
    }

    document.addEventListener('mousemove', move);
    document.addEventListener('mouseup', up);
    document.addEventListener('touchmove', move, { passive: false });
    document.addEventListener('touchend', up);

    move(e);
}

function drawHueSlider() {
    const grad = hueCtx.createLinearGradient(0, 0, hueCanvas.width, 0);

    for (let i = 0; i <= 360; i++) {
        grad.addColorStop(i / 360, `hsl(${i}, 100%, 50%)`);
    }

    hueCtx.fillStyle = grad;
    hueCtx.fillRect(0, 0, hueCanvas.width, hueCanvas.height);
    drawHueSelector();
}

function drawHueSelector() {
    hueCtx.lineWidth = 1;
    hueCtx.strokeStyle = 'black';
    hueCtx.strokeRect(hueSelectorX - 4, 0, 8, hueCanvas.height);
    hueCtx.strokeStyle = 'white';
    hueCtx.lineWidth = 2;
    hueCtx.strokeRect(hueSelectorX - 3, 1, 6, hueCanvas.height - 2);
}

function handleHueInteraction(e) {
    e.preventDefault();

    function move(ev) {
        const clientX = ev.clientX ?? ev.touches[0].clientX;
        const rect = hueCanvas.getBoundingClientRect();
        let x = clientX - rect.left;
        const scaleX = hueCanvas.width / rect.width;
        x = x * scaleX;

        x = Math.min(hueCanvas.width, Math.max(0, x));

        hueSelectorX = x;
        hue = (x / hueCanvas.width) * 360;

        drawHueSlider();
        drawPicker();
        updateOutput();
    }

    function up() {
        document.removeEventListener('mousemove', move);
        document.removeEventListener('mouseup', up);
        document.removeEventListener('touchmove', move);
        document.removeEventListener('touchend', up);
    }

    document.addEventListener('mousemove', move);
    document.addEventListener('mouseup', up);
    document.addEventListener('touchmove', move, { passive: false });
    document.addEventListener('touchend', up);

    move(e);
}

function hsvToRgb(h, s, v) {
    s /= 100; v /= 100;
    let c = v * s;
    let x = c * (1 - Math.abs((h / 60) % 2 - 1));
    let m = v - c;
    let r, g, b;
    if (h < 60) [r, g, b] = [c, x, 0];
    else if (h < 120) [r, g, b] = [x, c, 0];
    else if (h < 180) [r, g, b] = [0, c, x];
    else if (h < 240) [r, g, b] = [0, x, c];
    else if (h < 300) [r, g, b] = [x, 0, c];
    else[r, g, b] = [c, 0, x];
    return [
        Math.round((r + m) * 255),
        Math.round((g + m) * 255),
        Math.round((b + m) * 255)
    ];
}

function rgbToHex(r, g, b) {
    if (Array.isArray(r)) [r, g, b] = r;
    const clamp = v => Math.max(0, Math.min(255, Math.round(Number(v) || 0)));
    const toHex = v => clamp(v).toString(16).padStart(2, '0').toUpperCase();
    return `#${toHex(r)}${toHex(g)}${toHex(b)}`;
}

function shouldSendColorHex() {
    const now = Date.now();
    
    if (now > (colorHexLastSent + colorHexInterval)) {
        colorHexLastSent = now;
        return true;
    }
    return false;
}

function updateOutput() {
    const [r, g, b] = hsvToRgb(hue, sat, val);
    const hex = rgbToHex(r, g, b);
    outputRGB.textContent = `RGB: ${r}, ${g}, ${b}`;
    outputHex.textContent = `Hex: ${hex}`;

    // Ugly, who cares ;)
    if (isStartup) {
        isStartup = false;
        return;
    }
    
    if (shouldSendColorHex()) {
        sendColorHex(hex);
    }
}

function sendColorHex(hex) {
    hex = hex[0] === '#' ? hex.slice(1) : hex;

    // Validate
    if (!/^[0-9A-F]{6}$/.test(hex)) {
        console.log(`Invalid hex color: ${hex}`);
        return;
    }

    // Add '0x' prefix
    hex = '0x' + hex;
    currentHexColor = hex;

    fetch(`/color`, {
        method: 'POST',
        headers: { 'Content-Type': 'text/plain' },
        body: hex,
        cache: 'no-store'
    }).catch(() => { });
};
