const fs = require("fs");
const path = require("path");

const fw64 = require("./fw64");
const sgiTools = require("./sgi_tools");
const audioUtil = require("./audio_util");

const util = require("util");
const execFile = util.promisify(require('child_process').execFile);

const srcDir = "src";
const destDir = "dest";
const jobDir = "job";

async function main() {
    const outputPrefix = (process.argv.length >= 3) ? process.argv[2] : "music";

    const srcFiles = fs.readdirSync(srcDir);

    const sbkFile = path.join(destDir, `${outputPrefix}.sbk`)
    const sbcCommandArgs = [sgiTools.sbc, `-O${sbkFile}`];

    const wineScriptPath = path.join(jobDir, "winescript.sh");
    const wineScript = fs.openSync(wineScriptPath, "w");

    const filteredInsBasename = `${outputPrefix}.ins`;
    const filteredInsFile = path.join(fw64.generalMidiDir, filteredInsBasename);
    const json2insCommandArgs = [fw64.generalMidiJsonFile, filteredInsFile];

    const sequenceList = [];

    for (const file of srcFiles) {
        if (path.extname(file) !== ".mid")
            continue;

        const basename = path.basename(file, ".mid");
        const srcFile = path.join(srcDir, file);
        const midi0File = path.join(jobDir, file);
        const compressedMidiFile = path.join(jobDir, basename + ".cmp");

        const midicvtCommand = `wine ${sgiTools.midicvt} -o -s "${srcFile}" "${midi0File}"`;
        audioUtil.writeScriptCommand(wineScript, midicvtCommand);

        const midicompCommand = `wine ${sgiTools.midicomp} "${midi0File}" "${compressedMidiFile}"`;
        audioUtil.writeScriptCommand(wineScript, midicompCommand);

        sbcCommandArgs.push(compressedMidiFile);
        json2insCommandArgs.push(midi0File);
        sequenceList.push(basename);
    }

    fs.closeSync(wineScript);

    await audioUtil.runScript(wineScriptPath);

    // creates the compiled sbk file
    console.log("wine", sbcCommandArgs.join(' '));
    await execFile("wine", sbcCommandArgs);

    // creates a filtered ins file containing only the needed sounds for the created sequence bank
    console.log(fw64.json2ins, json2insCommandArgs.join(' '));
    await execFile(fw64.json2ins, json2insCommandArgs);

    // create the ctl and tbl files for the filtered instrument, then copy to output dir
    const icCommand = [sgiTools.ic, `-O${outputPrefix}`, filteredInsBasename];
    console.log("wine", icCommand);
    await execFile("wine", icCommand, {cwd: fw64.generalMidiDir});

    const ctlBasename = outputPrefix + ".ctl";
    const tblBasename = outputPrefix + ".tbl";
    fs.copyFileSync(path.join(fw64.generalMidiDir, ctlBasename), path.join(destDir, ctlBasename));
    fs.copyFileSync(path.join(fw64.generalMidiDir, tblBasename), path.join(destDir, tblBasename));

    const sequenceListPath = path.join(destDir, outputPrefix + ".json");
    fs.writeFileSync(sequenceListPath, JSON.stringify(sequenceList, null, 2));
}

if (require.main === module) {
    main();
}
