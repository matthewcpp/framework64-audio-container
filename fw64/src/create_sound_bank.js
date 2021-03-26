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
    const srcFiles = fs.readdirSync(srcDir);

    const wineScriptPath = path.join(jobDir, "winescript.sh");
    const wineScript = fs.openSync(wineScriptPath, "w");

    const soundBankData = new SoundBankData();

    for (const file of srcFiles) {
        const ext = path.extname(file).toLowerCase();

        if (!supportedTypes.has(ext))
            continue;

        const srcFile = path.join(srcDir, file);
        const aiffFile = path.join(jobDir, path.basename(file, ext) + ".aiff");

        const result = await execFile(fw64.snd2aiff, [srcFile, jobDir]);
        console.log(result.stdout);

        const aifcFile = audioUtil.compressAiff(wineScript, aiffFile, jobDir);
        soundBankData.addSound(path.basename(aifcFile));
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath);
    const jsonFilePath = path.join(jobDir, "bank.json");
    fs.writeFileSync(jsonFilePath, JSON.stringify(soundBankData.data, null, 2));

    const insFilePath = path.join(jobDir, "bank.ins");
    const json2insArgs = [jsonFilePath, insFilePath];
    console.log(fw64.json2ins, json2insArgs.join(' '));
    const jsdon2insResult = await execFile(fw64.json2ins, json2insArgs);
    console.log(jsdon2insResult.stdout);

    const icArgs = [sgiTools.ic, "-Osounds", "bank.ins"];
    console.log("wine", icArgs);
    const icResult = await execFile("wine", icArgs, {cwd: "/job"})
    console.log(icResult.stdout);

    fs.copyFileSync(path.join(jobDir, "sounds.ctl"), path.join(destDir, "sounds.ctl"));
    fs.copyFileSync(path.join(jobDir, "sounds.tbl"), path.join(destDir, "sounds.tbl"));
}

if (require.main === module) {
    main();
}
