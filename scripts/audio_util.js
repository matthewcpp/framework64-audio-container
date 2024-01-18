const fs = require("fs");
const path = require("path");

const sgiTools = require("./sgi_tools");

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

function compressAiff(wineScript, srcFile, destDir) {
    const baseName = path.basename(srcFile, ".aiff");
    const tableFile = path.join(destDir, baseName + ".table");
    const aifcFile = path.join(destDir, baseName + ".aifc")

    const tableCommand = `wine ${sgiTools.tabledesign} "${srcFile}" > "${tableFile}"`;
    writeScriptCommand(wineScript, tableCommand);

    const vadpcmCommand = `wine ${sgiTools.adpcmenc} -c "${tableFile}" "${srcFile}" "${aifcFile}"`
    writeScriptCommand(wineScript, vadpcmCommand);

    return aifcFile;
}

module.exports = {
    runScript: runScript,
    writeScriptCommand: writeScriptCommand,
    compressAiff: compressAiff,
};
