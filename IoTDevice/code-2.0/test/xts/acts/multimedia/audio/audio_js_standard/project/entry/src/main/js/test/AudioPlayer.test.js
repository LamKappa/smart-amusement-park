/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import media from '@ohos.multimedia.media';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
describe('AudioPlayer.test.js', function () {
    var audiosourcemp3 = 'file///build/testAudio/ff-16b-2c-44100hz.mp3';
    var audiosourcewav = 'file///build/testAudio/ff-16b-2c-44100hz.wav';
    var audiosourceaac = 'file///build/testAudio/ff-16b-2c-44100hz.aac';
    var audiosourceac3 = 'file///build/testAudio/ff-16b-2c-44100hz.ac3';
    var audiosourceaiff = 'file///build/testAudio/ff-16b-2c-44100hz.aiff';
    var audiosourceflac = 'file///build/testAudio/ff-16b-2c-44100hz.flac';
    var audiosourcem4a = 'file///build/testAudio/ff-16b-2c-44100hz.m4a';
    var audiosourcemp4 = 'file///build/testAudio/ff-16b-2c-44100hz.mp4';
    var audiosourceogg = 'file///build/testAudio/ff-16b-2c-44100hz.ogg';
    var audiosourceopus = 'file///build/testAudio/ff-16b-2c-44100hz.opus';
    var audiosourcets = 'file///build/testAudio/ff-16b-2c-44100hz.ts';
    var audiosourcewma = 'file///build/testAudio/ff-16b-2c-44100hz.wma';
    var docsource = 'file///build/testAudio/asc.doc';
    var wrongsource = 'file///abd';
    var audioPlayer = media.createAudioPlayer();
    var audioState;
    var waitTimer;

    beforeAll(function () {
        console.info('beforeAll: Prerequisites at the test suite level, which are executed before the test suite is executed.');
    })

    beforeEach(function () {
        console.info('beforeEach: Prerequisites at the test case level, which are executed before each test case is executed.');
    })

    afterEach(function () {
        console.info('afterEach: Test case-level clearance conditions, which are executed after each test case is executed.');
        audioPlayer.release();
    })

    afterAll(function () {
        console.info('afterAll: Test suite-level cleanup condition, which is executed after the test suite is executed');
    })

    console.info('Setting Callback');
    player.on('play', (err, action) => {
        if (err) {
            console.log(`err returned in play() callback`);
            return;
        }
        console.log(`Play() callback is called`);
        console.info('Current Player Status: ' + player.state);
        console.info('Current Song duration: ' + player.duration);
        console.log(`In play callback current time: ${player.currentTime}`);
        console.info('Pause aac');
        player.pause();
    });

    player.on('pause', (err, action) => {
        if (err) {
            console.log(`err returned in pause() callback`);
            return;
        }
        console.log(`Pause() callback is called`);
    });

    player.on('stop', (err, action) => {
        if (err) {
            console.log(`err returned in stop() callback`);
            return;
        }
        console.log(`Stop() callback is called`);
    });

    player.on('dataLoad', (err, action) => {
        if (err) {
            console.log(`err returned in dataLoad() callback`);
            return;
        }
        console.log(`dataLoad callback is called, cuurent time: ${player.currentTime}`);
        console.log(`Duration of the source: ${player.duration}`);
    });

    player.on('finish', (err, action) => {
        if (err) {
            console.log(`err returned in finish() callback`);
            return;
        }
        console.log(`Player finish callback is called`);
    });

    player.on('timeUpdate', (err, action) => {
        if (err) {
            console.log(`err returned in timeUpdate() callback`);
            return;
        }
        console.log(`In timeupdate callback current time: ${player.currentTime}`);
    });

    player.on('durationchange', (err, action) => {
        if (err) {
            console.log(`err returned in durationchange callback`);
            return;
        }
        console.log(`Durationchange callback is called`);
    });

    player.on('error', (err, action) => {
        console.error(`player error: ${err.code}`);
    });

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_001
        * @tc.name      : Set Audio Play 01
        * @tc.desc      : Audio Play-Positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
    */
    it('SUB_MEDIA_PLAYER_Play_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_002
        * @tc.name      : Audio play 02
        * @tc.desc      : Audio play-Error return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_002', 0, function () {
        audioPlayer.src = docsource;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertNotEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_002 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_003
        * @tc.name      : Audio play 03
        * @tc.desc      : Audio Play-Positive return value - wav
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_003', 0, function () {
        audioPlayer.src = audiosourcewav;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('SUB_MEDIA_PLAYER_Play_003 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_004
        * @tc.name      : Audio play 04
        * @tc.desc      : Audio Play-Positive return value - acc
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_004', 0, function () {
        audioPlayer.src = audiosourceaac;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_004 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_005
        * @tc.name      : Audio play 05
        * @tc.desc      : Audio Play-Positive return value - ac3
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_005', 0, function () {
        audioPlayer.src = audiosourceac3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_005 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_006
        * @tc.name      : Audio play 06
        * @tc.desc      : Audio Play-Positive return value - aiff
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_006', 0, function () {
        audioPlayer.src = audiosourceaiff;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_006 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_007
        * @tc.name      : Audio play 07
        * @tc.desc      : Audio Play-Positive return value - m4a
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_007', 0, function () {
        audioPlayer.src = audiosourcem4a;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_007 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_008
        * @tc.name      : Audio play 08
        * @tc.desc      : Audio Play-Positive return value - mp4
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_008', 0, function () {
        audioPlayer.src = audiosourcemp4;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_008 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_009
        * @tc.name      : Audio play 09
        * @tc.desc      : Audio Play-Positive return value - ogg
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_009', 0, function () {
        audioPlayer.src = audiosourceogg;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_009 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_010
        * @tc.name      : Audio play 10
        * @tc.desc      : Audio Play-Positive return value - opus
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_010', 0, function () {
        audioPlayer.src = audiosourceopus;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_010 :  PASS');

    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_011
        * @tc.name      : Audio play 11
        * @tc.desc      : Audio Play-Positive return value - ts
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_011', 0, function () {
        audioPlayer.src = audiosourcets;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_011 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_012
        * @tc.name      : Audio play 12
        * @tc.desc      : Audio Play-Positive return value - wma
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_012', 0, function () {
        audioPlayer.src = audiosourcewma;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_012 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Play_013
        * @tc.name      : Audio play 13
        * @tc.desc      : Audio Play-Positive return value - flac
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Play_013', 0, function () {
        audioPlayer.src = audiosourceflac;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Play_013 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Pause_001
        * @tc.name      : Audio Pause 01
        * @tc.desc      : Audio Pause-Positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Pause_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.pause();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('paused');
        console.info('testCase_SUB_MEDIA_PLAYER_Pause_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Pause_002
        * @tc.name      : Audio Pause 02
        * @tc.desc      : Audio Pause-Error return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Pause_002', 0, function () {
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('stopped');
        audioPlayer.pause();
        audioState = audioPlayer.state;
        expect(audioState).assertNotEqual('paused');
        console.info('testCase_SUB_MEDIA_PLAYER_Pause_002 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Stop_001
        * @tc.name      : Audio Stop 01
        * @tc.desc      : Audio Stop-Positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Stop_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('stopped');
        console.info('testCase_SUB_MEDIA_PLAYER_Stop_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Stop_002
        * @tc.name      : Audio Stop 02
        * @tc.desc      : Audio Stop-Error return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Stop_002', 0, function () {
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertNotEqual('stopped');
        console.info('testCase_SUB_MEDIA_PLAYER_Stop_002 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Seek_001
        * @tc.name      : Audio Seek 01
        * @tc.desc      : Audio Seek-Positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Seek_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        var timeUs = 10;
        audioPlayer.seek(timeUs);
        var getCurrentTime = audioPlayer.currenTime;
        if (getCurrentTime >= seekTime) {
            console.info("current position after seek : " + getCurrentTime);
            console.info('testCase_SUB_MEDIA_PLAYER_Seek_001 :  PASS');
        }
        else
        console.info('Error');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Seek_002
        * @tc.name      : Audio Seek 02
        * @tc.desc      : Audio Seek-error return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Seek_002', 0, function () {
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('stopped');
        audioPlayer.seek(10);
        var getCurrentTime = audioPlayer.currenTime;
        expect(getCurrentTime).assertNotEqual(10);
        console.info('testCase_SUB_MEDIA_PLAYER_Seek_002 :  PASS');

    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Seek_003
        * @tc.name      : Audio Seek 03
        * @tc.desc      : Audio Seek-Negative return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Seek_003', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        setTimeout(waitTimer, 2);
        expect(audioState).assertEqual('playing');
        audioPlayer.seek(-10);
        var getCurrentTime = audioPlayer.currenTime;
        expect(getCurrentTime).assertNotEqual(-10);
        console.info('testCase_SUB_MEDIA_PLAYER_Seek_003 :  PASS');

    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Seek_004
        * @tc.name      : Audio Seek 04
        * @tc.desc      : Audio Seek-out of range for the media its playing
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Seek_004', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.seek(16000);
        var getCurrentTime = audioPlayer.currenTime;
        expect(getCurrentTime).assertNotEqual(16000);
        console.info('testCase_SUB_MEDIA_PLAYER_Seek_004 :  PASS');

    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_Playback_status_001
        * @tc.name      : Audio Playback status 01
        * @tc.desc      : Audio Playback status-positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_Playback_status_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        setTimeout(waitTimer, 2);
        expect(audioState).assertEqual('playing');
        console.info('testCase_SUB_MEDIA_PLAYER_Playback_status_001 :  PASS');

    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_EnableLooping_001
        * @tc.name      : Audio EnableLooping 01
        * @tc.desc      : Audio Enable Looping after play
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_EnableLooping_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.loop = true;
        expect(audioPlayer.loop).assertEqual(true);
        console.info('testCase_SUB_MEDIA_PLAYER_EnableLooping_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_EnableLooping_002
        * @tc.name      : Audio EnableLooping 02
        * @tc.desc      : Audio EnableLooping- Enable Loop before play
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_EnableLooping_002', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.loop = true;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        expect(audioPlayer.loop).assertEqual(true);
        console.info('testCase_SUB_MEDIA_PLAYER_EnableLooping_002 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_EnableLooping_003
        * @tc.name      : Audio EnableLooping 03
        * @tc.desc      : Audio EnableLooping-Disable Loop
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_EnableLooping_003', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.loop = true;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.loop = false;
        expect(audioPlayer.loop).assertEqual(false);
        console.info('testCase_SUB_MEDIA_PLAYER_EnableLooping_003 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_SetVolume_001
        * @tc.name      : Audio SetVolume 01
        * @tc.desc      : Audio SetVolume-Positive return value
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_SetVolume_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.volume = 0.5;
        expect(audioPlayer.volume).assertEqual(0.5);
        console.info('testCase_SUB_MEDIA_PLAYER_SetVolume_001 :  PASS');
    })

    /* *
       * @tc.number    : SUB_MEDIA_PLAYER_SetVolume_002
        * @tc.name      : Audio SetVolume 02
        * @tc.desc      : Audio SetVolume-Mute
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_SetVolume_002', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.volume = 0;
        expect(setVolume).assertEqual(audioPlayer.volume);
        console.info('testCase_SUB_MEDIA_PLAYER_SetVolume_002 :  PASS');
    })

    /* *
       * @tc.number    : SUB_MEDIA_PLAYER_SetVolume_003
        * @tc.name      : Audio SetVolume 03
        * @tc.desc      : Audio SetVolume-Max
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_SetVolume_003', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        setTimeout(waitTimer, 2);
        expect(audioState).assertEqual('playing');
        audioPlayer.volume = 1;
        expect(audioPlayer.volume).assertEqual(1);
        console.info('testCase_SUB_MEDIA_PLAYER_SetVolume_003 :  PASS');
    })

    /* *
       * @tc.number    : SUB_MEDIA_PLAYER_SetVolume_004
        * @tc.name      : Audio SetVolume 04
        * @tc.desc      : Audio SetVolume-Out of range
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_SetVolume_004', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('playing');
        audioPlayer.volume = 2;
        expect(audioPlayer.volume).assertNotEqual(2);
        console.info('testCase_SUB_MEDIA_PLAYER_SetVolume_004 :  PASS');
    })

    /* *
       * @tc.number    : SUB_MEDIA_PLAYER_SetVolume_005
        * @tc.name      : Audio SetVolume 05
        * @tc.desc      : Audio SetVolume-negative range
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_SetVolume_005', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        audioPlayer.volume = -1;
        expect(audioPlayer.volume).assertNotEqual(-1);
        console.info('testCase_SUB_MEDIA_PLAYER_SetVolume_005 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_GetCurrentTime_001
        * @tc.name      : Audio GetCurrentTime 01
        * @tc.desc      : Audio GetCurrentTime-Play and get current time
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_GetCurrentTime_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        setTimeout(waitTimer, 20);
        expect(audioState).assertEqual('playing');
        var getCurrentTime = audioPlayer.currentTime;
        expect(getCurrentTime).assertEqual(20);
        console.info('testCase_SUB_MEDIA_PLAYER_GetCurrentTime_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_GetCurrentTime_002
        * @tc.name      : Audio GetCurrentTime 02
        * @tc.desc      : Audio GetCurrentTime- Error, get current time when nothing is playing.
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_GetCurrentTime_002', 0, function () {
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('stopped');
        var getCurrentTime = audioPlayer.currentTime;
        expect(getCurrentTime).assertNotEqual(0);
        console.info('testCase_SUB_MEDIA_PLAYER_GetCurrentTime_002 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_GetDuration_001
        * @tc.name      : Audio GetDuration 01
        * @tc.desc      : Audio GetDuration-Play and get duration.
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_GetDuration_001', 0, function () {
        audioPlayer.src = audiosourcemp3;
        audioPlayer.play();
        audioState = audioPlayer.state;
        setTimeout(waitTimer, 2);
        expect(audioState).assertEqual('playing');
        var getDuration = audioPlayer.duration;
        expect(getDuration).assertEqual(200);
        console.info('testCase_SUB_MEDIA_PLAYER_GetDuration_001 :  PASS');
    })

    /* *
        * @tc.number    : SUB_MEDIA_PLAYER_GetDuration_002
        * @tc.name      : Audio GetDuration 02
        * @tc.desc      : Audio GetDuration- Error, get duration time when nothing is playing.
        * @tc.size      : MEDIUM
        * @tc.type      : Function
        * @tc.level     : Level 0
        */
    it('SUB_MEDIA_PLAYER_GetDuration_002', 0, function () {
        audioPlayer.stop();
        audioState = audioPlayer.state;
        expect(audioState).assertEqual('stopped');
        var getDuration = audioPlayer.duration;
        expect(getDuration).assertNotEqual(0);
        console.info('testCase_SUB_MEDIA_PLAYER_GetDuration_002 :  PASS');
    })
})