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

open Protocol;

type amount =
  | Amount(Amount.t)
  | Illformed(string);

let keepToken = v =>
  v->Option.flatMap(
    fun
    | Illformed(_) => None
    | Amount(v) => v->Amount.getTez,
  );

let keepTez = v =>
  v->Option.flatMap(
    fun
    | Amount(v) => v->Amount.getTez
    | Illformed(_) => None,
  );

let parseAmount = (v, token: option(TokenRepr.t)) =>
  if (v == "") {
    None;
  } else {
    token->Option.mapWithDefault(
      {
        let vtez = v->Tez.fromString;
        vtez == None
          ? v->Illformed->Some
          : vtez->Option.map(v => v->Amount.makeTez->Amount);
      },
      t => {
        let vt = v->Token.Unit.fromStringDecimals(t.decimals);
        switch (vt) {
        | Error(_) => v->Illformed->Some
        | Ok(amount) => Amount.makeToken(~amount, ~token=t)->Amount->Some
        };
      },
    );
  };

let optToString = (v, f) => v->Option.mapWithDefault("", f);

module Alias = {
  type tempState =
    | NotAsked
    | Pending
    | Error(string);

  type t =
    | Address(PublicKeyHash.t)
    | Alias(Alias.t);

  type any =
    | Temp(string, tempState)
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

  let getAmount = v =>
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
    | Alias.Temp(_)
    | Alias.AnyString(_) => failwith("Should be an address or an alias")
    | Alias.Valid(a) => a;
};

let emptyOr = (f, v): ReSchema.fieldState => v == "" ? Valid : f(v);

let isValidTokenAmount: (string, int) => ReSchema.fieldState =
  (s, decimals) =>
    switch (s) {
    | "" => Error(I18n.Form_input_error.mandatory)
    | s =>
      switch (Token.Unit.fromStringDecimals(s, decimals)) {
      | Ok(_) => Valid
      | Error(Token.Unit.IllformedTokenUnit(_, NaN)) =>
        Error(I18n.Form_input_error.float)
      | Error(Token.Unit.IllformedTokenUnit(_, Negative)) =>
        Error(I18n.Form_input_error.float)
      | Error(Token.Unit.IllformedTokenUnit(_, Float(_))) =>
        Error(I18n.Form_input_error.expected_decimals(decimals))
      | Error(e) => Error(e->Errors.toString)
      }
    };

let isValidTezAmount: string => ReSchema.fieldState =
  s => isValidTokenAmount(s, 6);

let notNone = (v): ReSchema.fieldState =>
  v != None ? Valid : Error(I18n.Form_input_error.mandatory);

let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN
      ? Error(I18n.Form_input_error.float) : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->int_of_string_opt->Option.isSome
      ? Valid : Error(I18n.Form_input_error.int);
  fieldState;
};

let checkDerivationPath = (s): ReSchema.fieldState =>
  switch (s->DerivationPath.Pattern.fromString) {
  | Ok(_) => Valid
  | Error(e) => Error(Errors.toString(e))
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

let getFormStateError = (formState: ReForm.formState) =>
  switch (formState) {
  | SubmitFailed(error) => error
  | _ => None
  };

let checkAddress = (v): ReSchema.fieldState =>
  switch (v->PublicKeyHash.build) {
  | Ok(_) => Valid
  | Error(e) => Error(Errors.toString(e))
  };

let i18n = {
  ...ReSchemaI18n.default,
  stringNonEmpty: (~value as _) => I18n.Form_input_error.mandatory,
};
