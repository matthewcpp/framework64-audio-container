const fs = require("fs")
const { spawn } = require('child_process');

async function runScript(path) {
    return new Promise((resolve) => {
        const process = spawn('sh', [path]);

        process.stdout.on('data', (data) => {
            console.log(`${data}`);
        });

        process.stderr.on('data', (data) => {
            console.error(`${data}`);
        });

        process.on('close', (code) => {
            resolve(code);
        });
    })
}

function writeScriptCommand(wineScript, command) {
    fs.writeSync(wineScript, `echo '${command}'\n`);
    fs.writeSync(wineScript, command + '\n');
}

module.exports = {
    runScript: runScript,
    writeScriptCommand: writeScriptCommand
};
