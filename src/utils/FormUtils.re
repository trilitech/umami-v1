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

open Transfer;

type amount =
  | Amount(Transfer.currency)
  | Illformed(string);

let keepToken = v =>
  v->Option.flatMap(
    fun
    | Illformed(_) => None
    | Amount(v) => v->Transfer.getTez,
  );

let keepTez = v =>
  v->Option.flatMap(
    fun
    | Amount(v) => v->Transfer.getTez
    | Illformed(_) => None,
  );

let parseAmount = (v, token) =>
  if (v == "") {
    None;
  } else {
    token->Option.mapWithDefault(
      {
        let vtez = v->Tez.fromString;
        vtez == None
          ? v->Illformed->Some
          : vtez->Option.map(v => v->Transfer.makeTez->Amount);
      },
      t => {
        let vt = v->Token.Unit.fromNatString;
        vt == None
          ? v->Illformed->Some
          : vt->Option.map(amount => makeToken(~amount, ~token=t)->Amount);
      },
    );
  };

let optToString = (v, f) => v->Option.mapWithDefault("", f);

module Alias = {
  type t =
    | Address(string)
    | Alias(Alias.t);

  type any =
    | AnyString(string)
    | Valid(t);

  let address =
    fun
    | Address(s) => s
    | Alias(a) => a.address;

  let alias =
    fun
    | Address(_) => ""
    | Alias(a) => a.name;
};

module Unsafe = {
  // more explicit than assert(false)

  let getValue =
    fun
    | None => failwith("Should not be None")
    | Some(v) => v;

  let getCurrency = v =>
    switch (v) {
    | None => failwith("Should not be empty")
    | Some(Illformed(_)) => failwith("Should not be malformed")
    | Some(Amount(a)) => a
    };

  let getTez = v =>
    switch (v) {
    | Amount(Tez(a)) => a
    | Illformed(_)
    | Amount(Token(_)) => failwith("Should not be malformed")
    };

  let account =
    fun
    | Alias.AnyString(_) => failwith("Should be an address or an alias")
    | Alias.Valid(a) => a;
};

let emptyOr = (f, v): ReSchema.fieldState => v == "" ? Valid : f(v);

let isValidTezAmount: string => ReSchema.fieldState =
  fun
  | s when Tez.fromString(s) != None => Valid
  | "" => Error(I18n.form_input_error#mandatory)
  | _ => Error(I18n.form_input_error#float);

let isValidTokenAmount: string => ReSchema.fieldState =
  fun
  | s when Token.Unit.forceFromString(s) != None => Valid
  | "" => Error(I18n.form_input_error#mandatory)
  | _ => Error(I18n.form_input_error#int);

let notNone = (v): ReSchema.fieldState =>
  v != None ? Valid : Error(I18n.form_input_error#mandatory);

let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN
      ? Error(I18n.form_input_error#float) : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->int_of_string_opt->Option.isSome
      ? Valid : Error(I18n.form_input_error#int);
  fieldState;
};

let formFieldsAreValids = (fieldsState, validateFields) => {
  let fields = fieldsState->Array.map(((field, _)) => field);
  let fieldsState = validateFields(fields);

  fieldsState->Array.every((fieldState: ReForm.fieldState) => {
    switch (fieldState) {
    | Valid => true
    | _ => false
    }
  });
};

let i18n = {
  ...ReSchemaI18n.default,
  stringNonEmpty: (~value as _) => I18n.form_input_error#mandatory,
};
