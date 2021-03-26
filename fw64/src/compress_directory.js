const fs = require("fs");
const path = require("path")

const sgiTools = require("./sgi_tools");
const audioUtil = require("./audio_util");

const srcDir = "/src";
const destDir = "/dest"
const jobDir = "/job";

async function main() {
    const wineScriptPath = path.join(jobDir, "winescript.sh")
    const wineScript = fs.openSync(wineScriptPath, "w");

    let count = 0;
    const sourceFiles = fs.readdirSync(srcDir);
    for (const file of sourceFiles) {
        if (path.extname(file) === ".aiff") {
            audioUtil.compressAiff(wineScript, path.join(srcDir, file), jobDir);
        }

        if (count > 10) break;
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath)
}

if (require.main === module) {
    main();
}
