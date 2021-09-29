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

open UmamiCommon;

module FileReader = {
  type t;
  [@bs.new] external createFileReader: unit => t = "FileReader";

  [@bs.send] external readAsText: (t, Js.t('a)) => unit = "readAsText";

  [@bs.set]
  external onload: (t, ReactEvent.Media.t => unit) => unit = "onload";
};

[@bs.send] external click: Dom.element => unit = "click";

[@react.component]
let make = (~text, ~primary=?, ~onChange as onChangeResult, ~accept=?) => {
  let inputRef = React.useRef(Js.Nullable.null);

  let onPress = _ => {
    inputRef.current->Js.Nullable.toOption->Lib.Option.iter(click);
  };

  let onChange = event => {
    let file = event->ReactEvent.Form.target##files->Array.get(0);
    switch (file) {
    | Some(file) =>
      let fileReader = FileReader.createFileReader();
      fileReader->FileReader.onload(event => {
        onChangeResult(event->ReactEvent.Media.target##result)
      });
      fileReader->FileReader.readAsText(file);
    | None => ()
    };
  };

  <>
    <ButtonAction onPress text ?primary icon=Icons.ArrowDown.build />
    <input
      ref={inputRef->ReactDOM.Ref.domRef}
      type_="file"
      multiple=false
      ?accept
      style={ReactDOM.Style.make(~display="none", ())}
      onChange
    />
  </>;
};
