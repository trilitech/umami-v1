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

type t;

type event;

[@bs.module "electron"] external renderer: t = "ipcRenderer";

[@bs.send] external on: (t, string, (event, string) => unit) => unit = "on";
[@bs.send]
external removeListener: (t, string, (event, string) => unit) => unit =
  "removeListener";
[@bs.send] external send: (t, string) => unit = "send";

let useNextDeeplinkState = () => {
  let (next, done_, send) = ReactUtils.useNextState();
  React.useEffect0(() => {
    let listener = (_, message) => {
      send(message);
    };
    renderer->on("deeplinkURL", listener);
    Some(_ => renderer->removeListener("deeplinkURL", listener));
  });
  (next, done_);
};

let deeplinkPrefix = "umami://";

let useDeeplink = () => {
  let (state, setState) = React.useState(() => None);

  React.useEffect0(() => {
    let listener = (_, url) => {
      let message =
        url->Js.String2.substr(~from=deeplinkPrefix->Js.String.length);

      setState(_ => Some(message));
    };
    renderer->on("deeplinkURL", listener);
    Some(_ => renderer->removeListener("deeplinkURL", listener));
  });

  state;
};
