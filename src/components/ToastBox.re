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

open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~width=600.->dp,
          ~position=`absolute,
          ~bottom=20.->dp,
          ~left=150.->dp,
          ~display=`flex,
          ~flexDirection=`columnReverse,
          ~zIndex=10000,
          (),
        ),
    })
  );

module Item = {
  [@react.component]
  let make = (~indice, ~handleDelete, ~log, ~addToast) => {
    let fadeAnim = React.useRef(Animated.Value.create(0.)).current;

    React.useEffect1(
      () => {
        ReactUtils.startFade(fadeAnim, 1., 800., None);
        None;
      },
      [|fadeAnim|],
    );

    <Animated.View
      style=Style.(style(~opacity=fadeAnim->Animated.StyleProp.float, ()))>
      <LogItem.Toast indice log addToast handleDelete />
    </Animated.View>;
  };
};

[@react.component]
let make = (~opacity, ~logs, ~addToast, ~firsts) => {
  let (hide, setHide) = React.useState(() => []);
  let handleDelete = (tm, _) => setHide(hide => [tm, ...hide]);

  <Animated.View
    style=Style.([|styles##container, style(~opacity, ())|]->array)>
    {logs
     ->List.firsts(firsts)
     ->List.reverse
     ->List.toArray
     ->Array.mapWithIndex((i, log) =>
         if (hide->List.has(log.Logs.timestamp, (==))) {
           React.null;
         } else {
           let handleDelete = handleDelete(log.Logs.timestamp);
           <Item key={i->string_of_int} indice=i log handleDelete addToast />;
         }
       )
     ->React.array}
  </Animated.View>;
};
