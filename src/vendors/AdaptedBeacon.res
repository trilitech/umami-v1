module LegacyError = {
  open Beacon.Error

  // Unduplicate this
  type Errors.t +=
    | NoMatchingRequest
    | EncodedPayloadNeedString
    | MessageNotHandled
    | CouldNotDecryptMessage
    | AppMetadataNotFound
    | ShouldNotWork
    | ContainerNotFound
    | PlatformUnknown
    | PairingRequestParsing

  // this is coupled to umami errors
  let parse = e =>
    switch e.RawJsError.message {
    | s if s->Js.String2.includes(noMatchingRequest) => NoMatchingRequest
    | s if s->Js.String2.includes(encodedPayloadNeedString) => EncodedPayloadNeedString
    | s if s->Js.String2.includes(messageNotHandled) => MessageNotHandled
    | s if s->Js.String2.includes(couldNotDecryptMessage) => CouldNotDecryptMessage
    | s if s->Js.String2.includes(appMetadataNotFound) => AppMetadataNotFound
    | s if s->Js.String2.includes(shouldNotWork) => ShouldNotWork
    | s if s->Js.String2.includes(containerNotFound) => ContainerNotFound
    | s if s->Js.String2.includes(platformUnknown) => PlatformUnknown
    | s => Errors.Generic(Js.String.make(s))
    }

  let fromPromiseParsed = p => p->RawJsError.fromPromiseParsed(parse)
}
