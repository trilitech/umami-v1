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

open ReactNative

module Form = {
  module StateLenses = %lenses(type state = {pairingRequest: string})

  include ReForm.Make(StateLenses)
}

let styles = {
  open Style
  StyleSheet.create({
    "title": style(~marginBottom=8.->dp, ~textAlign=#center, ()),
    "verticalFormAction": StyleSheet.flatten([
      FormStyles.verticalFormAction,
      style(~marginTop=12.->dp, ()),
    ]),
  })
}

@react.component
let make = (~closeAction) => {
  let updatePeers = StoreContext.Beacon.Peers.useResetAll()

  let (client, _) = StoreContext.Beacon.useClient()

  let form = Form.use(
    ~schema={
      open Form.Validation
      Schema(nonEmpty(PairingRequest))
    },
    ~onSubmit=({state, raiseSubmitFailed}) => {
      let pairingInfo = ReBeacon.parsePairingRequest(state.values.pairingRequest)

      switch pairingInfo {
      | Ok(pairingInfo) =>
        client
        ->Promise.fromOption(~error=Errors.Generic(I18n.Errors.beacon_client_not_created))
        ->Promise.flatMapOk(client => client->ReBeacon.WalletClient.addPeer(pairingInfo))
        ->Promise.get(x =>
          switch x {
          | Error(e) => raiseSubmitFailed(Some(e->Errors.toString))
          | Ok(_) =>
            updatePeers()
            closeAction()
          }
        )

      | Error(error) => raiseSubmitFailed(Some(error->Errors.toString))
      }

      None
    },
    ~initialState={pairingRequest: ""},
    ~i18n=FormUtils.i18n,
    (),
  )

  let formFieldsAreValids = FormUtils.formFieldsAreValids(form.fieldsState, form.validateFields)

  let submitOnEnterKeyPress = event =>
    if (event->TextInput.KeyPressEvent.nativeEvent)["key"] === "Enter" {
      TextInput.KeyPressEvent.preventDefault(event)
      form.submit()
    }

  <ModalFormView closing=ModalFormView.Close(closeAction)>
    <View>
      <View style=FormStyles.header>
        <Typography.Headline style={styles["title"]}>
          {I18n.Title.beacon_pairing->React.string}
        </Typography.Headline>
      </View>
      <FormGroupTextInput
        label=I18n.Label.beacon_dapp_pairing
        value=form.values.pairingRequest
        placeholder=j`e.g. BSdNU2tFbvtHvFpWR7rjrHyna1VQkAFnz4CmDTqkohdCx4FS51WUpc5Z9YoNJqbtZpoDNJfencTaDp23fWQqcyL54F75puvwCfmC1RCn11RLyFHrCYKo7uJ7a9KR8txqb1712J78ZXpLEvjbALAacLPrrvcJxta6XpU8Cd6F8NUHqBGd2Y4oWD9iQnyXB7umC72djzJFJVEgN5Z37DdiXPscqCMs7mX6qpuhq8thyKCDVhkvT9sr9t5EU7LYMxUHJgDdBS8K2GfTf76NTrHNV9AqjWcbbGM4EpPtGjsB8g6DjoH3xTdAtb9GE1PB2pFvucUMWrdT`
        handleChange={form.handleChange(PairingRequest)}
        error={list{
          form.formState->FormUtils.getFormStateError,
          form.getFieldError(Field(PairingRequest)),
        }->Option.firstSome}
        onKeyPress=submitOnEnterKeyPress
        multiline=true
        numberOfLines=9
      />
      <View style={styles["verticalFormAction"]}>
        <Buttons.SubmitPrimary
          text=I18n.Btn.beacon_connect_dapp
          onPress={_ => form.submit()}
          disabledLook={!formFieldsAreValids}
        />
      </View>
    </View>
  </ModalFormView>
}

module WithQR = {
  module QRScanner = {
    @val external window: 'a = "window"

    type rafID
    @val
    external requestAnimationFrame: (unit => unit) => rafID = "requestAnimationFrame"
    @val
    external cancelAnimationFrame: rafID => unit = "cancelAnimationFrame"

    module Image = {
      type t
      @get
      external data: t => Js.Typed_array.Uint8ClampedArray.t = "data"
      @get external height: t => float = "height"
      @get external width: t => float = "width"
    }

    module Canvas2d = {
      type t
      @send
      external drawElement: (
        t,
        ~element: Dom.element,
        ~dx: float,
        ~dy: float,
        ~dWidth: float,
        ~dHeight: float,
      ) => unit = "drawImage"
      @send
      external getImageData: (t, ~sx: float, ~sy: float, ~sw: float, ~sh: float) => Image.t =
        "getImageData"
    }

    module CanvasElement = {
      @send
      external getContext2d: (Dom.element, @as("2d") _) => Canvas2d.t = "getContext"
      @get external height: Dom.element => int = "height"
      @set external setHeight: (Dom.element, int) => unit = "height"
      @get external width: Dom.element => int = "width"
      @set external setWidth: (Dom.element, int) => unit = "width"
    }

    type Errors.t +=
      | StreamError

    let () = Errors.registerHandler("Media", x =>
      switch x {
      | StreamError => I18n.Errors.stream->Some
      | _ => None
      }
    )

    module VideoElement = {
      type state
      @get external readyState: Dom.element => state = "readyState"
      @get
      external haveEnoughData: Dom.element => state = "HAVE_ENOUGH_DATA"
      @get external videoHeight: Dom.element => int = "videoHeight"
      @get external videoWidth: Dom.element => int = "videoWidth"
      @set external setSrcObject: (Dom.element, 'a) => unit = "srcObject"
      @send
      external setAttribute: (Dom.element, string, bool) => unit = "setAttribute"
      @send external play: Dom.element => unit = "play"
    }

    @react.component
    let make = (~onQRCodeData) => {
      let videoRef = React.useRef(window["document"]["createElement"]("video"))
      let canvasRef = React.useRef(Js.Nullable.null)
      let rafRef = React.useRef(Js.Nullable.null)

      let (hasStream, setHasStream) = React.useState(_ => false)

      let rec tick = () => {
        let videoElement = videoRef.current
        switch canvasRef.current->Js.Nullable.toOption {
        | Some(canvasElement) =>
          let canvas = canvasElement->CanvasElement.getContext2d
          if videoElement->VideoElement.readyState == videoElement->VideoElement.haveEnoughData {
            canvasElement->CanvasElement.setHeight(videoElement->VideoElement.videoHeight)
            canvasElement->CanvasElement.setWidth(videoElement->VideoElement.videoWidth)
            canvas->Canvas2d.drawElement(
              ~element=videoElement,
              ~dx=0.,
              ~dy=0.,
              ~dWidth=canvasElement->CanvasElement.width->float_of_int,
              ~dHeight=canvasElement->CanvasElement.height->float_of_int,
            )
            let imageData =
              canvas->Canvas2d.getImageData(
                ~sx=0.,
                ~sy=0.,
                ~sw=canvasElement->CanvasElement.width->float_of_int,
                ~sh=canvasElement->CanvasElement.height->float_of_int,
              )
            let qrCode = {
              open JsQR
              scan(
                imageData->Image.data,
                imageData->Image.width,
                imageData->Image.height,
                options(~inversionAttempts=#dontInvert, ()),
              )
            }

            switch qrCode->Js.Nullable.toOption {
            | Some(qrCode) => onQRCodeData(qrCode.data)
            | None => ()
            }
          }
        | None => ()
        }

        let raf = requestAnimationFrame(tick)
        rafRef.current = Js.Nullable.return(raf)
        ()
      }

      React.useEffect0(() => {
        let streamRef = ref(None)

        Promise.async(() =>
          window["navigator"]["mediaDevices"]["getUserMedia"]({
            "video": {
              "facingMode": "environment",
            },
          })
          ->Promise.fromJs(_ => StreamError)
          ->Promise.mapOk(stream => {
            setHasStream(_ => true)
            streamRef := Some(stream)
            videoRef.current->VideoElement.setSrcObject(stream)
            videoRef.current->VideoElement.setAttribute("playsinline", true)
            videoRef.current->VideoElement.play
            let raf = requestAnimationFrame(tick)
            rafRef.current = Js.Nullable.return(raf)
          })
        )

        Some(
          () => {
            // stop the camera streaming
            switch streamRef.contents {
            | Some(stream) =>
              let tracks = stream["getTracks"]()
              tracks["forEach"](track => track["stop"]())
              ()
            | None => ()
            }
            // stop requestAnimationFrame loop
            switch rafRef.current->Js.Nullable.toOption {
            | Some(raf) => raf->cancelAnimationFrame
            | None => ()
            }
          },
        )
      })

      <View>
        {hasStream
          ? <canvas
              ref={canvasRef->ReactDOM.Ref.domRef}
              style={ReactDOM.Style.make(
                ~objectFit="cover",
                ~width="372px",
                ~height="372px",
                ~borderRadius="4px",
                ~transform="scaleX(-1)",
                (),
              )}
            />
          : <Typography.Overline2 style=FormStyles.textAlignCenter>
              {I18n.Errors.video_stream_access_denied->React.string}
            </Typography.Overline2>}
      </View>
    }
  }

  @react.component
  let make = (~closeAction) => {
    let addToast = LogsContext.useToast()
    let (webcamScanning, setWebcamScanning) = React.useState(_ => true)
    let updatePeers = StoreContext.Beacon.Peers.useResetAll()
    let (client, _) = StoreContext.Beacon.useClient()

    let onQRCodeData = dataUrl => {
      setWebcamScanning(_ => false)
      let data = dataUrl->Js.String2.replace("tezos://?type=tzip10&data=", "")

      let pairingInfo = ReBeacon.parsePairingRequest(data)

      switch pairingInfo {
      | Ok(pairingInfo) =>
        Promise.async(() =>
          client
          ->Promise.fromOption(~error=BeaconApiRequest.ClientNotConnected)
          ->Promise.flatMapOk(client =>
            client
            ->ReBeacon.WalletClient.addPeer(pairingInfo)
            ->Promise.tapError(error => {
              addToast(Logs.error(~origin=Beacon, error))
              setWebcamScanning(_ => true)
            })
          )
          ->Promise.mapOk(() => {
            updatePeers()
            closeAction()
          })
        )
      | Error(error) =>
        addToast(Logs.error(~origin=Beacon, error))
        setWebcamScanning(_ => true)
      }

      ()
    }

    <ModalFormView closing=ModalFormView.Close(closeAction)>
      <View>
        <View style=FormStyles.header>
          <Typography.Headline> {I18n.Title.beacon_pairing_qr->React.string} </Typography.Headline>
          <Typography.Overline1 style=FormStyles.subtitle>
            {I18n.Title.beacon_scan_qr->React.string}
          </Typography.Overline1>
        </View>
        {webcamScanning
          ? <QRScanner onQRCodeData />
          : <View
              style={
                open Style
                style(~width=372.->dp, ~height=372.->dp, ())
              }>
              <LoadingView size=ActivityIndicator_Size.large />
            </View>}
      </View>
    </ModalFormView>
  }
}
