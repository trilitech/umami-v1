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

type t = ..;

type scope = string;

type errorInfos = {
  msg: string,
  scope: string,
};

type t +=
  | Generic(string)
  | WrongPassword;

let handlers = ref([]);

let registerHandler = (scope, f) => {
  handlers := [(scope, f), ...handlers.contents];
};

let () =
  registerHandler(
    "Generic",
    fun
    | Generic(s) => s->Some
    | WrongPassword => I18n.form_input_error#wrong_password->Some
    | _ => None,
  );

let getInfos = e => {
  let infos =
    handlers.contents
    ->List.findMap(((scope, h)) => e->h->Option.map(msg => {scope, msg}));

  switch (infos) {
  | Some(i) => i
  | None =>
    /* We know by construction that [e] is an object as it is of type [t]. It
       must be convertible to json by [stringifyAny] */
    {
      scope: "Generic",
      msg:
        I18n.errors#unhandled_error(e->Js.Json.stringifyAny->Option.getExn),
    }
  };
};

let toString = e => getInfos(e).msg;
