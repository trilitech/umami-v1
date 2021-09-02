/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

const conf = require("../package.json");
const fs = require('fs/promises');
const { spawn } = require('child_process');

let name = conf.name;
let version = conf.version;
let description = conf.description;
let mimeTypes = conf.build.linux.mimeTypes;
let resources = conf.build.directories.buildResources;

let snap_tmpl = "scripts/snap-tmpl.yaml";
let desktop_tmpl = "scripts/snap-tmpl.desktop";

let snap_yaml = "snap/snapcraft.yaml"
let desktop = "snap/gui/" + name + ".desktop"
let icon = "snap/gui/icon.png"

function replaceAll (v) {
    return v
        .replaceAll("%%NAME%%", name)
        .replaceAll("%%VERSION%%", version)
        .replaceAll("%%SUMMARY%%", description)
        .replaceAll("%%DESCRIPTION%%", description)
        .replaceAll("%%COMMENT%%", description)
        .replaceAll("%%MIMETYPES%%", mimeTypes.join(";") + ";");
}

async function mkdirs() {
    await fs.mkdir("snap/gui", { recursive : true })
        .catch(error => {
            console.log("Error:" + JSON.stringify(error));
            process.exit(2);
        });
}

async function filltmpl (tmpl, res) {
    let content = await fs.readFile(tmpl, "utf8")
        .then(content => { return replaceAll(content) })
        .catch(error => {
            console.log("Error when filling " + tmpl + " :" + JSON.stringify(error));
            process.exit(2);
        });

    await fs.writeFile(res, content)
        .catch(error => {
            console.log("Error when writing " + res + " :" + JSON.stringify(error));
            process.exit(2);
        });
}

async function copyIcon () {

    await fs.copyFile(resources + "/icon.png", icon)
        .catch(error => {
            console.log("Error when copying icon from " + resources + " :" +
                        JSON.stringify(error));
            process.exit(2);
        })

}

async function generateSnapFiles () {
    await mkdirs();
    await filltmpl(snap_tmpl, snap_yaml);
    await filltmpl(desktop_tmpl, desktop);
    await copyIcon();
}

async function buildSnap() {
    const snapcraft = spawn('snapcraft', ['--use-lxd']);

    snapcraft.stdout.on('data', (data) => {
        console.log(`${data}`);
    });

    snapcraft.stderr.on('data', (data) => {
        console.error(`${data}`);
    });

    snapcraft.on('close', (code) => {
        console.log(`child process exited with code ${code}`);
    });

    snapcraft.on('error', (err) => {
        console.error('Failed to start snapcraft subprocess.');
        console.error(err);
    });

};

async function generateConf () {
    console.log("Generating snap configuration files");
    await generateSnapFiles();
}

exports.conf = generateConf;

exports.default = async function snap (context) {
    if (context.electronPlatformName !== 'linux') {
        return;
    }

    generateConf();
    console.log("Generating snap");
    await buildSnap();
}
