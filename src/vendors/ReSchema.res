/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

module type Lenses = {
  type field<'a>
  type state
  let set: (state, field<'a>, 'a) => state
  let get: (state, field<'a>) => 'a
}

type childFieldError = {
  error: string,
  index: int,
  name: string,
}

type fieldState =
  | Valid
  | NestedErrors(array<childFieldError>)
  | Error(string)

type recordValidationState<'a> =
  | Valid
  | Errors(array<('a, fieldState)>)

module Make = (Lenses: Lenses) => {
  type rec field = Field(Lenses.field<'a>): field

  module Validation = {
    type rec t<'meta> =
      | Email({field: Lenses.field<string>, error: option<string>, meta: 'meta}): t<'meta>
      | NoValidation({field: Lenses.field<'a>, meta: 'b}): t<'meta>
      | StringNonEmpty({field: Lenses.field<string>, error: option<string>, meta: 'meta}): t<'meta>
      | StringRegExp({
          field: Lenses.field<string>,
          matches: string,
          error: option<string>,
          meta: 'meta,
        }): t<'meta>
      | StringMin({
          field: Lenses.field<string>,
          min: int,
          error: option<string>,
          meta: 'meta,
        }): t<'meta>
      | StringMax({
          field: Lenses.field<string>,
          max: int,
          error: option<string>,
          meta: 'meta,
        }): t<'meta>
      | IntMin({field: Lenses.field<int>, min: int, error: option<string>, meta: 'meta}): t<'meta>
      | IntMax({field: Lenses.field<int>, max: int, error: option<string>, meta: 'meta}): t<'meta>
      | FloatMin({
          field: Lenses.field<float>,
          min: float,
          error: option<string>,
          meta: 'meta,
        }): t<'meta>
      | FloatMax({
          field: Lenses.field<float>,
          max: float,
          error: option<string>,
          meta: 'meta,
        }): t<'meta>
      | Custom({
          field: Lenses.field<'a>,
          meta: 'meta,
          predicate: Lenses.state => fieldState,
        }): t<'meta>
      | True({field: Lenses.field<bool>, error: option<string>, meta: 'meta}): t<'meta>
      | False({field: Lenses.field<bool>, error: option<string>, meta: 'meta}): t<'meta>
    type rec schema<'meta> = Schema(array<t<'meta>>): schema<'meta>

    let \"+" = (a, b) => a->Array.concat(b)
    let \"<?" = (arr, maybeArr) =>
      switch maybeArr {
      | Some(someArr) => arr + [someArr]
      | None => arr
      }

    let custom = (predicate, ~meta=?, field) => [
      Custom({field: field, meta: meta, predicate: predicate}),
    ]

    let true_ = (~error=?, ~meta=?, field) => [True({field: field, meta: meta, error: error})]

    let false_ = (~error=?, ~meta=?, field) => [False({field: field, meta: meta, error: error})]

    let email = (~error=?, ~meta=?, field) => [Email({field: field, meta: meta, error: error})]

    let nonEmpty = (~error=?, ~meta=?, field) => [
      StringNonEmpty({field: field, meta: meta, error: error}),
    ]

    let string = (~min=?, ~minError=?, ~max=?, ~maxError=?, ~meta=?, field) => {
      open Option
      \"<?"(
        \"<?"(
          [],
          min->map(min => StringMin({field: field, meta: meta, min: min, error: minError})),
        ),
        max->map(max => StringMax({field: field, meta: meta, max: max, error: maxError})),
      )
    }

    let regExp = (~error=?, ~matches, ~meta=?, field) => [
      StringRegExp({field: field, meta: meta, matches: matches, error: error}),
    ]

    let float = (~min=?, ~minError=?, ~max=?, ~maxError=?, ~meta=?, field) => {
      open Option
      \"<?"(
        \"<?"([], min->map(min => FloatMin({field: field, meta: meta, min: min, error: minError}))),
        max->map(max => FloatMax({field: field, meta: meta, max: max, error: maxError})),
      )
    }

    let int = (~min=?, ~minError=?, ~max=?, ~maxError=?, ~meta=?, field) => {
      open Option
      \"<?"(
        \"<?"([], min->map(min => IntMin({field: field, meta: meta, min: min, error: minError}))),
        max->map(max => IntMax({field: field, meta: meta, max: max, error: maxError})),
      )
    }
  }

  module RegExps = {
    let email = %re(`/.*@.*\\..+/`)
  }

  let validateField = (~validator, ~values, ~i18n: ReSchemaI18n.t): (field, fieldState) =>
    switch validator {
    | Validation.True({field, error}) =>
      let value = Lenses.get(values, field)
      (Field(field), value ? Valid : Error(error->Option.getWithDefault(i18n.true_())))
    | Validation.False({field, error}) =>
      let value = Lenses.get(values, field)
      (Field(field), value == false ? Valid : Error(error->Option.getWithDefault(i18n.false_())))
    | Validation.IntMin({field, min, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        value >= min ? Valid : Error(error->Option.getWithDefault(i18n.intMin(~value, ~min))),
      )
    | Validation.IntMax({field, max, error}) =>
      let value = Lenses.get(values, field)

      (
        Field(field),
        value <= max ? Valid : Error(error->Option.getWithDefault(i18n.intMax(~value, ~max))),
      )
    | Validation.FloatMin({field, min, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        value >= min ? Valid : Error(error->Option.getWithDefault(i18n.floatMin(~value, ~min))),
      )
    | Validation.FloatMax({field, max, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        Lenses.get(values, field) <= max
          ? Valid
          : Error(error->Option.getWithDefault(i18n.floatMax(~value, ~max))),
      )
    | Validation.Email({field, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        Js.Re.test_(RegExps.email, value)
          ? Valid
          : Error(error->Option.getWithDefault(i18n.email(~value))),
      )
    | Validation.NoValidation({field}) => (Field(field), Valid)
    | Validation.StringNonEmpty({field, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        value === "" ? Error(error->Option.getWithDefault(i18n.stringNonEmpty(~value))) : Valid,
      )
    | Validation.StringRegExp({field, matches, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        Js.Re.test_(Js.Re.fromString(matches), value)
          ? Valid
          : Error(error->Option.getWithDefault(i18n.stringRegExp(~value, ~pattern=matches))),
      )
    | Validation.StringMin({field, min, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        Js.String.length(value) >= min
          ? Valid
          : Error(error->Option.getWithDefault(i18n.stringMin(~value, ~min))),
      )
    | Validation.StringMax({field, max, error}) =>
      let value = Lenses.get(values, field)
      (
        Field(field),
        Js.String.length(value) <= max
          ? Valid
          : Error(error->Option.getWithDefault(i18n.stringMax(~value, ~max))),
      )
    | Validation.Custom({field, predicate}) => (Field(field), predicate(values))
    }

  let getFieldValidators = (~validators, ~fieldName) =>
    validators->Array.keep(validator =>
      switch validator {
      | Validation.False({field}) => Field(field) == fieldName
      | Validation.True({field}) => Field(field) == fieldName
      | Validation.IntMin({field}) => Field(field) == fieldName
      | Validation.IntMax({field}) => Field(field) == fieldName
      | Validation.FloatMin({field}) => Field(field) == fieldName
      | Validation.FloatMax({field}) => Field(field) == fieldName
      | Validation.Email({field}) => Field(field) == fieldName
      | Validation.NoValidation({field}) => Field(field) == fieldName
      | Validation.StringNonEmpty({field}) => Field(field) == fieldName
      | Validation.StringRegExp({field}) => Field(field) == fieldName
      | Validation.StringMin({field}) => Field(field) == fieldName
      | Validation.StringMax({field}) => Field(field) == fieldName
      | Validation.Custom({field}) => Field(field) == fieldName
      }
    )

  let validateOne = (~field, ~values, ~i18n, schema: Validation.schema<'meta>) => {
    let Validation.Schema(validators) = schema

    getFieldValidators(~validators, ~fieldName=field)
    ->Array.map(validator => validateField(~validator, ~values, ~i18n))
    ->Array.getBy(fieldStateValidated =>
      switch fieldStateValidated {
      | (_, Error(_)) => true
      | (_, NestedErrors(_)) => true
      | _ => false
      }
    )
  }

  let validateFields = (~fields, ~values, ~i18n, schema: Validation.schema<'meta>) => {
    let Validation.Schema(validators) = schema

    Array.map(fields, field =>
      getFieldValidators(~validators, ~fieldName=field)
      ->Array.map(validator => validateField(~validator, ~values, ~i18n))
      ->Array.getBy(fieldStateValidated =>
        switch fieldStateValidated {
        | (_, Error(_)) => true
        | (_, NestedErrors(_)) => true
        | _ => false
        }
      )
    )
  }

  let validate = (
    ~i18n=ReSchemaI18n.default,
    values: Lenses.state,
    schema: Validation.schema<'meta>,
  ) => {
    let Validation.Schema(validators) = schema

    let validationList =
      validators->Array.map(validator => validateField(~validator, ~values, ~i18n))

    let errors = validationList->Array.keepMap(((field, fieldState)) =>
      switch fieldState {
      | Error(_) => Some((field, fieldState))
      | NestedErrors(_) => Some((field, fieldState))
      | _ => None
      }
    )

    Array.length(errors) > 0 ? Errors(errors) : Valid
  }
}
