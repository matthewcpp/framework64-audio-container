# framework64-audio-container
This repository contains code and tools required to build the [matthewcpp/framework64-audio](https://hub.docker.com/r/matthewcpp/framework64-audio) container which provides a convenient cross platform approach to preparing audio for use with [framework64](https://github.com/matthewcpp/framework64) and the [NuSystem SGI Audio tools](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/index.htm).

The repository makes use of the SGI audio tools for compressing audio and creating sound bank files.  The N64 gneral midi soundfont is used for sequences.  Currently the sequence banks created by this container are compressed for use with the [Compact Sequence Player](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/n64man/al/alCSeqPlayer.htm).  This is the default player for NuSystem's SGI Audio tools.

### Creating Sound Effect Banks

The `create_sound_bank` option will create a a sound bank from all sounds in a directory.  The supported types are: `.wav`, `.flac`, `.ogg`, or `.aiff`. Stereo files will be re-sampled to mono during the conversion process.

To create a sound effect bank, place your audio files into a folder and run the following command:

```shell
docker run --rm -v /path/to/input/dir:/src -v path/to/output/dir:/dest matthewcpp/framework64-audio create_sound_bank [bank_name=sounds]
```

If the operation is successful, three files will be created in your output directory: `bank_name.ctl`, `bank_name.tbl`, and `bank_name.json`. The `.ctl` and `.tbl` files can be used with [nuAuSndPlayerBankSet](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/nuAuSndPlayerBankSet.htm). The `.json` file contains the list of files that were encoded into the bank.  The index of the sound effect in the array corresponds to the `sndNo` parameter of [nuAuSndPlayerPlay](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/nuAuSndPlayerPlay.htm).

### Creating a Sequence Bank

The `create_sequence_bank` option will create a sequence bank and corresponding instrument bank for all midi files in a directory. Type 1 midi files will be copied and converted to type 0 during processing. Input files will not be modified in any way.  The resulting instrument bank will contain only the sounds needed to play the midi files in the supplied input folder.

To create a sequence bank, place your midi files into a folder and run the following command:
```shell
docker run --rm -v /path/to/input/dir:/src -v path/to/output/dir:/dest matthewcpp/framework64-audio create_sequence_bank [bank_name=music]
```

if the operation is successfull, four files will be created in your output directory: `bank_name.ctl`, `bank_name.tbl`, `bank_name.sbk` and `bank_name.json`. The `.ctl` and `.tbl` files can be used with [nuAuSeqPlayerBankSet](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/nuAuSeqPlayerBankSet.htm) to set the sequence player instrument bank. The `.sbk` file is used with [nuAuSeqPlayerSeqSet](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/nuAuSeqPlayerSeqSet.htm) to load the sequence bank into the compact player. The `.json` file contains the list of midi files that were encoded into the bank.  The index of the midi file in the array corresponds to the `seq_no` parameter of [nuAuSeqPlayerSetNo](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/nusystem/nu_f/audio_sgi/nuAuSeqPlayerSetNo.htm).

### Building

To build the container from the root repository:
```shell
docker build -t matthewcpp/framework64-audio .
```