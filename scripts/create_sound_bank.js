const fs = require("fs");
const path = require("path");

const fw64 = require("./fw64");
const sgiTools = require("./sgi_tools");
const audioUtil = require("./audio_util");
const SoundBankData = require("./sound_bank_data");

const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const srcDir = "src";
const destDir = "dest";
const jobDir = "job";

const supportedTypes = new Set([".ogg", ".wav", ".aiff", ".flac"]);

async function main() {
    const outputPrefix = (process.argv.length >= 3) ? process.argv[2] : "sounds";

    const srcFiles = fs.readdirSync(srcDir);

    const wineScriptPath = path.join(jobDir, "winescript.sh");
    const wineScript = fs.openSync(wineScriptPath, "w");

    const soundBankData = new SoundBankData();
    const soundList = [];

    for (const file of srcFiles) {
        const ext = path.extname(file).toLowerCase();

        if (!supportedTypes.has(ext))
            continue;

        const basename = path.basename(file, ext);
        const srcFile = path.join(srcDir, file);
        const aiffFile = path.join(jobDir, basename + ".aiff");

        const result = await execFile(fw64.snd2aiff, [srcFile, jobDir]);
        console.log(result.stdout);

        const aifcFile = audioUtil.compressAiff(wineScript, aiffFile, jobDir);
        soundBankData.addSound(path.basename(aifcFile));
        soundList.push(basename);
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath);
    const jsonFilePath = path.join(jobDir, outputPrefix + ".json");
    fs.writeFileSync(jsonFilePath, JSON.stringify(soundBankData.data, null, 2));

    const insFileName = outputPrefix + ".ins";
    const insFilePath = path.join(jobDir, insFileName);
    const json2insArgs = [jsonFilePath, insFilePath];
    console.log(fw64.json2ins, json2insArgs.join(' '));
    const jsdon2insResult = await execFile(fw64.json2ins, json2insArgs);
    console.log(jsdon2insResult.stdout);

    const icArgs = [sgiTools.ic, `-O${outputPrefix}`, insFileName];
    console.log("wine", icArgs);
    const icResult = await execFile("wine", icArgs, {cwd: "/job"})
    console.log(icResult.stdout);

    const ctlBasename = outputPrefix + ".ctl";
    const tblBasename = outputPrefix + ".tbl";
    fs.copyFileSync(path.join(jobDir, ctlBasename), path.join(destDir, ctlBasename));
    fs.copyFileSync(path.join(jobDir, tblBasename), path.join(destDir, tblBasename));

    const soundListPath = path.join(destDir, outputPrefix + ".json");
    fs.writeFileSync(soundListPath, JSON.stringify(soundList, null, 2));
}

if (require.main === module) {
    main();
}
