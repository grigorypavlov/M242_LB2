let allWindows;
let locked = true;
let offsetX, offsetY;
let selectedWindow;

const getCursorPosition = (e) => {
    cursorX = (window.Event) ? e.pageX :
        event.clientX + (document.documentElement.scrollLeft ?
            document.documentElement.scrollLeft :
            document.body.scrollLeft);

    cursorY = (window.Event) ? e.pageY :
        event.clientY + (document.documentElement.scrollTop ?
            document.documentElement.scrollTop :
            document.body.scrollTop);

    return {
        cursorX,
        cursorY
    };
};

const windowToCursorPosition = (w) => {
    w.style.position = "absolute";
    w.style.top = cursorY - offsetY + "px";
    w.style.left = cursorX - offsetX + "px";
};

const grabWindow = (e, w) => {
    if (locked) return;

    // Prevent text selection.
    e.preventDefault();

    // Calculate mouse offset inside window.
    const target = w;
    const rect = target.getBoundingClientRect();
    offsetX = e.clientX - rect.left;
    offsetY = e.clientY - rect.top;

    selectedWindow = w;
};

const onWindowMoving = (e) => {
    const cursor = getCursorPosition(e);

    if (selectedWindow != null) {
        windowToCursorPosition(selectedWindow);
    }
};

const releaseWindow = (e) => {
    selectedWindow = null;
};

const lock = () => {
    locked = true;

    for (const w of allWindows) {
        w.style.cursor = "auto";
    }
};

const unlock = () => {
    locked = false;

    for (const w of allWindows) {
        w.style.cursor = "move";
    }
};

// Register event handlers.
const registerWindows = (windows) => {
    for (const w of windows) {
        w.onmousedown = (e) => {
            grabWindow(e, w);
        };
    }

    document.onmousemove = onWindowMoving;

    document.onmouseup = releaseWindow;

    allWindows = windows;
};
