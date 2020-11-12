open ReSchema;

module Helpers = ReForm__Helpers;

module type Config = {
  type field('a);
  type state;
  let set: (state, field('a), 'a) => state;
  let get: (state, field('a)) => 'a;
};

type fieldState =
  | Pristine
  | Valid
  | NestedErrors(array(ReSchema.childFieldError))
  | Error(string);

type formState =
  | Dirty
  | Submitting
  | Pristine
  | Errored
  | SubmitFailed(option(string))
  | Valid;

module Make = (Config: Config) => {
  module ReSchema = ReSchema.Make(Config);
  module Validation = ReSchema.Validation;

  type field = ReSchema.field;
  type nonrec formState = formState;

  type action =
    | ValidateField(field)
    | ValidateForm(bool)
    | TrySubmit
    | Submit
    | SetFieldsState(array((field, fieldState)))
    | FieldChangeState(field, fieldState)
    | FieldChangeValue(Config.field('a), 'a): action
    | FieldChangeValueWithCallback(Config.field('a), 'a => 'a): action
    | FieldArrayAdd(Config.field(array('a)), 'a): action
    | FieldArrayUpdateByIndex(Config.field(array('a)), 'a, int): action
    | FieldArrayRemove(Config.field(array('a)), int): action
    | FieldArrayRemoveBy(Config.field(array('a)), 'a => bool): action
    | SetFormState(formState)
    | ResetForm
    | SetValues(Config.state)
    | SetFieldValue(Config.field('a), 'a): action
    | RaiseSubmitFailed(option(string));

  type state = {
    formState,
    values: Config.state,
    fieldsState: array((field, fieldState)),
  };

  type api = {
    state,
    values: Config.state,
    formState,
    fieldsState: array((field, fieldState)),
    isSubmitting: bool,
    isDirty: bool,
    isPristine: bool,
    getFieldState: field => fieldState,
    getFieldError: field => option(string),
    getNestedFieldError: (field, int) => option(string),
    handleChange: 'a. (Config.field('a), 'a) => unit,
    handleChangeWithCallback: 'a. (Config.field('a), 'a => 'a) => unit,
    arrayPush: 'a. (Config.field(array('a)), 'a) => unit,
    arrayUpdateByIndex:
      'a.
      (~field: Config.field(array('a)), ~index: int, 'a) => unit,

    arrayRemoveByIndex: 'a. (Config.field(array('a)), int) => unit,
    arrayRemoveBy: 'a. (Config.field(array('a)), 'a => bool) => unit,
    submit: unit => unit,
    resetForm: unit => unit,
    setValues: Config.state => unit,
    setFieldValue:
      'a.
      (Config.field('a), 'a, ~shouldValidate: bool=?, unit) => unit,

    validateField: field => unit,
    validateForm: unit => unit,
    validateFields: array(field) => array(fieldState),
    raiseSubmitFailed: option(string) => unit,
  };

  type onSubmitAPI = {
    send: action => unit,
    state,
    raiseSubmitFailed: option(string) => unit,
  };

  type fieldInterface('value) = {
    handleChange: 'value => unit,
    error: option(string),
    state: fieldState,
    validate: unit => unit,
    value: 'value,
  };

  type validationStrategy =
    | OnChange
    | OnDemand;

  let getInitialFieldsState:
    Validation.schema('meta) => array((field, fieldState)) =
    schema => {
      let Validation.Schema(validators) = schema;
      validators->Belt.Array.map(validator =>
        switch (validator) {
        | Validation.IntMin({field}) => (
            ReSchema.Field(field),
            Pristine: fieldState,
          )
        | Validation.True({field}) => (Field(field), Pristine)
        | Validation.False({field}) => (Field(field), Pristine)
        | Validation.IntMax({field}) => (Field(field), Pristine)
        | Validation.FloatMin({field}) => (Field(field), Pristine)
        | Validation.FloatMax({field}) => (Field(field), Pristine)
        | Validation.Email({field}) => (Field(field), Pristine)
        | Validation.NoValidation({field}) => (Field(field), Pristine)
        | Validation.StringNonEmpty({field}) => (Field(field), Pristine)
        | Validation.StringRegExp({field}) => (Field(field), Pristine)
        | Validation.StringMin({field}) => (Field(field), Pristine)
        | Validation.StringMax({field}) => (Field(field), Pristine)
        | Validation.Custom({field}) => (Field(field), Pristine)
        }
      );
    };

  let formContext: React.Context.t(option(api)) = React.createContext(None);

  let useFormContext = () => React.useContext(formContext);
  let useField = field => {
    let interface = useFormContext();
    interface->Belt.Option.map(
      ({handleChange, getFieldError, getFieldState, validateField, state}) =>
      {
        handleChange: handleChange(field),
        error: getFieldError(Field(field)),
        state: getFieldState(Field(field)),
        validate: () => validateField(Field(field)),
        value: state.values->Config.get(field),
      }
    );
  };

  module Provider = {
    let makeProps = (~value, ~children, ()) => {
      "value": Some(value),
      "children": children,
    };
    let make = React.Context.provider(formContext);
  };

  module Field = {
    [@react.component]
    let make =
        (
          ~field: Config.field('a),
          ~render: fieldInterface('a) => React.element,
          ~renderOnMissingContext=React.null,
          (),
        ) => {
      let fieldInterface = useField(field);
      React.useMemo3(
        () =>
          fieldInterface
          ->Belt.Option.map(render)
          ->Belt.Option.getWithDefault(renderOnMissingContext),
        (
          Belt.Option.(fieldInterface->map(({error}) => error)),
          Belt.Option.(fieldInterface->map(({value}) => value)),
          Belt.Option.(fieldInterface->map(({state}) => state)),
        ),
      );
    };
  };

  let use =
      (
        ~initialState,
        ~schema: Validation.schema('meta),
        ~onSubmit,
        ~onSubmitFail=ignore,
        ~i18n=ReSchemaI18n.default,
        ~validationStrategy=OnChange,
        (),
      ) => {
    let (state, send) =
      ReactUpdate.useReducer(
        {
          fieldsState: getInitialFieldsState(schema),
          values: initialState,
          formState: Pristine,
        },
        (action, state) =>
        switch (action) {
        | Submit =>
          UpdateWithSideEffects(
            {...state, formState: Submitting},
            self =>
              onSubmit({
                send: self.send,
                state: self.state,
                raiseSubmitFailed: error =>
                  self.send(RaiseSubmitFailed(error)),
              }),
          )
        | TrySubmit =>
          SideEffects(
            self => {
              self.send(ValidateForm(true));
              None;
            },
          )
        | SetFieldsState(fieldsState) => Update({...state, fieldsState})
        | ValidateField(field) =>
          SideEffects(
            self => {
              let fieldState =
                schema
                |> ReSchema.validateOne(
                     ~field,
                     ~values=self.state.values,
                     ~i18n,
                   );
              let newFieldState: fieldState =
                switch (fieldState) {
                | None => Valid
                | Some(fieldState) =>
                  switch (fieldState) {
                  | (_, Error(message)) => Error(message)
                  | (_, NestedErrors(errors)) => NestedErrors(errors)
                  | (_, Valid) => Valid
                  }
                };

              let newFieldsState =
                state.fieldsState
                ->Belt.Array.keep(elem =>
                    elem
                    |> (((fieldValue, _fieldState)) => fieldValue != field)
                  )
                ->Belt.Array.concat([|(field, newFieldState)|]);
              self.send(SetFieldsState(newFieldsState));
              None;
            },
          )
        | ValidateForm(submit) =>
          SideEffects(
            self => {
              let recordState =
                schema |> ReSchema.validate(~i18n, self.state.values);

              switch (recordState) {
              | Valid =>
                self.send(SetFormState(Valid));
                submit ? self.send(Submit) : ();
              | Errors(erroredFields) =>
                let newFieldsState: array((field, fieldState)) =
                  erroredFields->Belt.Array.map(((field, errorMsg)) =>
                    (field, Error(errorMsg))
                  );
                self.send(SetFieldsState(newFieldsState));
                submit
                  ? onSubmitFail({
                      send: self.send,
                      state: {
                        ...self.state,
                        fieldsState: newFieldsState,
                      },
                      raiseSubmitFailed: error =>
                        self.send(RaiseSubmitFailed(error)),
                    })
                  : ();
                self.send(SetFormState(Errored));
              };
              None;
            },
          )
        | FieldChangeValue(field, value) =>
          UpdateWithSideEffects(
            {
              ...state,
              formState: state.formState == Errored ? Errored : Dirty,
              values: Config.set(state.values, field, value),
            },
            self => {
              switch (validationStrategy) {
              | OnChange => self.send(ValidateField(Field(field)))
              | OnDemand => ()
              };
              None;
            },
          )
        | FieldChangeValueWithCallback(field, updateFn) =>
          let oldValue = Config.get(state.values, field);
          UpdateWithSideEffects(
            {
              ...state,
              formState: state.formState == Errored ? Errored : Dirty,
              values: Config.set(state.values, field, updateFn(oldValue)),
            },
            self => {
              switch (validationStrategy) {
              | OnChange => self.send(ValidateField(Field(field)))
              | OnDemand => ()
              };
              None;
            },
          );
        | FieldChangeState(_, _) => NoUpdate
        | FieldArrayAdd(field, entry) =>
          Update({
            ...state,
            values:
              Config.set(
                state.values,
                field,
                Belt.Array.concat(
                  Config.get(state.values, field),
                  [|entry|],
                ),
              ),
          })
        | FieldArrayRemove(field, index) =>
          Update({
            ...state,
            values:
              Config.set(
                state.values,
                field,
                Config.get(state.values, field)
                ->Belt.Array.keepWithIndex((_, i) => i != index),
              ),
          })
        | FieldArrayRemoveBy(field, predicate) =>
          Update({
            ...state,
            values:
              Config.set(
                state.values,
                field,
                Config.get(state.values, field)
                ->Belt.Array.keep(entry => !predicate(entry)),
              ),
          })
        | FieldArrayUpdateByIndex(field, value, index) =>
          Update({
            ...state,
            values:
              Config.set(
                state.values,
                field,
                Config.get(state.values, field)
                ->Belt.Array.mapWithIndex((i, currentValue) =>
                    i == index ? value : currentValue
                  ),
              ),
          })
        | SetFormState(newState) => Update({...state, formState: newState})
        | ResetForm =>
          Update({
            fieldsState: getInitialFieldsState(schema),
            values: initialState,
            formState: Pristine,
          })
        | SetValues(values) => Update({...state, values})
        | SetFieldValue(field, value) =>
          Update({...state, values: Config.set(state.values, field, value)})
        | RaiseSubmitFailed(err) =>
          Update({...state, formState: SubmitFailed(err)})
        }
      );

    let getFieldState = field =>
      state.fieldsState
      ->Belt.Array.getBy(((nameField, _nameFieldState)) =>
          nameField == field
        )
      ->Belt.Option.mapWithDefault(
          Pristine: fieldState, ((_nameField, nameFieldState)) =>
          nameFieldState
        );

    let getFieldError = field =>
      getFieldState(field)
      |> (
        fun
        | Error(error) => Some(error)
        | NestedErrors(_errors) => {
            Js.log2(
              "The following field has nested errors, access these with `getNestedFieldError` instead of `getFieldError`",
              field,
            );
            None;
          }
        | Pristine
        | Valid => None
      );

    let validateFields = (fields: array(field)) => {
      let fieldsValidated =
        ReSchema.validateFields(~fields, ~values=state.values, ~i18n, schema);

      let newFieldsState =
        Belt.Array.map(
          state.fieldsState,
          fieldStateItem => {
            let (field, _) = fieldStateItem;

            Belt.Array.some(fields, fieldItem => fieldItem == field)
              ? {
                let newFieldState =
                  fieldsValidated->Belt.Array.getBy(fieldStateValidated =>
                    Belt.Option.map(fieldStateValidated, ((item, _)) => item)
                    == Some(field)
                  );

                newFieldState
                ->Belt.Option.getWithDefault(None)
                ->Belt.Option.mapWithDefault(
                    [||], ((_, newFieldStateValidated)) =>
                    switch (newFieldStateValidated) {
                    | Valid => [|(field, Valid: fieldState)|]
                    | Error(message) => [|(field, Error(message))|]
                    | NestedErrors(message) => [|
                        (field, NestedErrors(message)),
                      |]
                    }
                  );
              }
              : [|fieldStateItem|];
          },
        )
        ->Belt.Array.concatMany;

      send(SetFieldsState(newFieldsState));

      Belt.Array.keepMap(newFieldsState, ((field, fieldState)) => {
        Belt.Array.some(fields, fieldItem => fieldItem == field)
          ? Some(fieldState) : None
      });
    };

    let raiseSubmitFailed = error => send(RaiseSubmitFailed(error));

    let getNestedFieldError = (field, index) =>
      getFieldState(field)
      |> (
        fun
        | NestedErrors(errors) => {
            switch (errors->Belt.Array.get(index)) {
            | None => None
            | Some(error) => Some(error.error)
            };
          }
        | Pristine
        | Valid
        | Error(_) => None
      );

    let interface: api = {
      state,
      formState: state.formState,
      fieldsState: state.fieldsState,
      values: state.values,
      isSubmitting: state.formState == Submitting,
      isDirty: state.formState == Dirty,
      isPristine: state.formState == Pristine,
      submit: () => send(TrySubmit),
      resetForm: () => send(ResetForm),
      setValues: values => send(SetValues(values)),
      setFieldValue: (field, value, ~shouldValidate=true, ()) => {
        shouldValidate
          ? send(FieldChangeValue(field, value))
          : send(SetFieldValue(field, value));
      },
      getFieldState,
      getFieldError,
      getNestedFieldError,
      handleChange: (field, value) => send(FieldChangeValue(field, value)),
      handleChangeWithCallback: (field, updateFn) =>
        send(FieldChangeValueWithCallback(field, updateFn)),
      arrayPush: (field, value) => send(FieldArrayAdd(field, value)),
      arrayUpdateByIndex: (~field, ~index, value) =>
        send(FieldArrayUpdateByIndex(field, value, index)),
      arrayRemoveBy: (field, predicate) =>
        send(FieldArrayRemoveBy(field, predicate)),
      arrayRemoveByIndex: (field, index) =>
        send(FieldArrayRemove(field, index)),
      validateField: field => send(ValidateField(field)),
      validateForm: () => send(ValidateForm(false)),
      validateFields,
      raiseSubmitFailed,
    };

    interface;
  };
};
