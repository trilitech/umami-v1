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

let mapOpt = (v, f) => {
  v->Option.mapWithDefault(React.null, f);
};

let hideNil = (v, f) =>
  switch (v) {
  | [] => React.null
  | v => f(v)
  };

let opt = e =>
  switch (e) {
  | Some(e) => e
  | None => React.null
  };

let displayOn = b => Style.(style(~display=b ? `flex : `none, ()));

let visibleOn = b => Style.(style(~opacity=b ? 1. : 0., ()));

let onlyWhen = (elt, b) => b ? elt : React.null;

let startFade = (refval, endval, duration, endCallback) => {
  Animated.(
    Value.Timing.(
      timing(
        refval,
        config(
          ~toValue=fromRawValue(endval),
          ~duration,
          ~useNativeDriver=true,
          (),
        ),
      )
      ->start(~endCallback?, ())
    )
  );
};

let useIsMonted = () => {
  let (isMounted, setIsMounted) = React.useState(_ => false);
  React.useEffect1(
    () => {
      setIsMounted(_ => true);
      None;
    },
    [|setIsMounted|],
  );
  isMounted;
};

let styles = l => Style.array(List.toArray(l));
