
function reserved2(arg) {
    return arg < 10 ? "0" + arg.toString() : arg.toString();
}

function time2string(arg) {
    let num = Math.floor(arg);
    let seconds = num % 60;
    let minutes = Math.floor(num / 60);
    let str = reserved2(minutes) + ":" + reserved2(seconds);
    return str;
}
