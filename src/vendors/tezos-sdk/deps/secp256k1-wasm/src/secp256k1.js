
var _SECP256K1 = (function() {
  var _scriptDir = typeof document !== 'undefined' && document.currentScript ? document.currentScript.src : undefined;
  if (typeof __filename !== 'undefined') _scriptDir = _scriptDir || __filename;
  return (
    function(_SECP256K1) {
      _SECP256K1 = _SECP256K1 || {};

      var Module = typeof _SECP256K1 !== "undefined" ? _SECP256K1 : {};
      var readyPromiseResolve, readyPromiseReject;
      Module["ready"] = new Promise(function(resolve, reject) {
        readyPromiseResolve = resolve;
        readyPromiseReject = reject
      });
      var moduleOverrides = {};
      var key;
      for (key in Module) {
        if (Module.hasOwnProperty(key)) {
          moduleOverrides[key] = Module[key]
        }
      }
      var arguments_ = [];
      var thisProgram = "./this.program";
      var quit_ = function(status, toThrow) {
        throw toThrow
      };
      var ENVIRONMENT_IS_WEB = false;
      var ENVIRONMENT_IS_WORKER = false;
      var ENVIRONMENT_IS_NODE = false;
      var ENVIRONMENT_IS_SHELL = false;
      ENVIRONMENT_IS_WEB = typeof window === "object";
      ENVIRONMENT_IS_WORKER = typeof importScripts === "function";
      ENVIRONMENT_IS_NODE = typeof process === "object" && typeof process.versions === "object" && typeof process.versions.node === "string";
      ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;
      var scriptDirectory = "";

      function locateFile(path) {
        if (Module["locateFile"]) {
          return Module["locateFile"](path, scriptDirectory)
        }
        return "./" + path;
      }
      var read_, readAsync, readBinary, setWindowTitle;
      var nodeFS;
      var nodePath;
      if (ENVIRONMENT_IS_NODE) {
        if (ENVIRONMENT_IS_WORKER) {
          scriptDirectory = require("path").dirname(scriptDirectory) + "/"
        } else {
          scriptDirectory = __dirname + "/"
        }
        read_ = function shell_read(filename, binary) {
          if (!nodeFS) nodeFS = require("fs");
          if (!nodePath) nodePath = require("path");
          filename = nodePath["normalize"](filename);
          return nodeFS["readFileSync"](filename, binary ? null : "utf8")
        };
        readBinary = function readBinary(filename) {
          var ret = read_(filename, true);
          if (!ret.buffer) {
            ret = new Uint8Array(ret)
          }
          assert(ret.buffer);
          return ret
        };
        if (process["argv"].length > 1) {
          thisProgram = process["argv"][1].replace(/\\/g, "/")
        }
        arguments_ = process["argv"].slice(2);
        process["on"]("uncaughtException", function(ex) {
          if (!(ex instanceof ExitStatus)) {
            throw ex
          }
        });
        process["on"]("unhandledRejection", abort);
        quit_ = function(status) {
          process["exit"](status)
        };
        Module["inspect"] = function() {
          return "[Emscripten Module object]"
        }
      } else if (ENVIRONMENT_IS_SHELL) {
        if (typeof read != "undefined") {
          read_ = function shell_read(f) {
            return read(f)
          }
        }
        readBinary = function readBinary(f) {
          var data;
          if (typeof readbuffer === "function") {
            return new Uint8Array(readbuffer(f))
          }
          data = read(f, "binary");
          assert(typeof data === "object");
          return data
        };
        if (typeof scriptArgs != "undefined") {
          arguments_ = scriptArgs
        } else if (typeof arguments != "undefined") {
          arguments_ = arguments
        }
        if (typeof quit === "function") {
          quit_ = function(status) {
            quit(status)
          }
        }
        if (typeof print !== "undefined") {
          if (typeof console === "undefined") console = {};
          console.log = print;
          console.warn = console.error = typeof printErr !== "undefined" ? printErr : print
        }
      } else if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
        if (ENVIRONMENT_IS_WORKER) {
          scriptDirectory = self.location.href
        } else if (document.currentScript) {
          scriptDirectory = document.currentScript.src
        }
        if (_scriptDir) {
          scriptDirectory = _scriptDir
        }
        if (scriptDirectory.indexOf("blob:") !== 0) {
          scriptDirectory = scriptDirectory.substr(0, scriptDirectory.lastIndexOf("/") + 1)
        } else {
          scriptDirectory = ""
        } {
          read_ = function shell_read(url) {
            var xhr = new XMLHttpRequest;
            xhr.open("GET", url, false);
            xhr.send(null);
            return xhr.responseText
          };
          if (ENVIRONMENT_IS_WORKER) {
            readBinary = function readBinary(url) {
              var xhr = new XMLHttpRequest;
              xhr.open("GET", url, false);
              xhr.responseType = "arraybuffer";
              xhr.send(null);
              return new Uint8Array(xhr.response)
            }
          }
          readAsync = function readAsync(url, onload, onerror) {
            var xhr = new XMLHttpRequest;
            xhr.open("GET", url, true);
            xhr.responseType = "arraybuffer";
            xhr.onload = function xhr_onload() {
              if (xhr.status == 200 || xhr.status == 0 && xhr.response) {
                onload(xhr.response);
                return
              }
              onerror()
            };
            xhr.onerror = onerror;
            xhr.send(null)
          }
        }
        setWindowTitle = function(title) {
          document.title = title
        }
      } else {}
      var out = Module["print"] || console.log.bind(console);
      var err = Module["printErr"] || console.warn.bind(console);
      for (key in moduleOverrides) {
        if (moduleOverrides.hasOwnProperty(key)) {
          Module[key] = moduleOverrides[key]
        }
      }
      moduleOverrides = null;
      if (Module["arguments"]) arguments_ = Module["arguments"];
      if (Module["thisProgram"]) thisProgram = Module["thisProgram"];
      if (Module["quit"]) quit_ = Module["quit"];
      var wasmBinary;
      if (Module["wasmBinary"]) wasmBinary = Module["wasmBinary"];
      var noExitRuntime;
      if (Module["noExitRuntime"]) noExitRuntime = Module["noExitRuntime"];
      if (typeof WebAssembly !== "object") {
        abort("no native wasm support detected")
      }
      var wasmMemory;
      var wasmTable;
      var ABORT = false;
      var EXITSTATUS = 0;

      function assert(condition, text) {
        if (!condition) {
          abort("Assertion failed: " + text)
        }
      }
      var UTF8Decoder = typeof TextDecoder !== "undefined" ? new TextDecoder("utf8") : undefined;

      function UTF8ArrayToString(heap, idx, maxBytesToRead) {
        var endIdx = idx + maxBytesToRead;
        var endPtr = idx;
        while (heap[endPtr] && !(endPtr >= endIdx)) ++endPtr;
        if (endPtr - idx > 16 && heap.subarray && UTF8Decoder) {
          return UTF8Decoder.decode(heap.subarray(idx, endPtr))
        } else {
          var str = "";
          while (idx < endPtr) {
            var u0 = heap[idx++];
            if (!(u0 & 128)) {
              str += String.fromCharCode(u0);
              continue
            }
            var u1 = heap[idx++] & 63;
            if ((u0 & 224) == 192) {
              str += String.fromCharCode((u0 & 31) << 6 | u1);
              continue
            }
            var u2 = heap[idx++] & 63;
            if ((u0 & 240) == 224) {
              u0 = (u0 & 15) << 12 | u1 << 6 | u2
            } else {
              u0 = (u0 & 7) << 18 | u1 << 12 | u2 << 6 | heap[idx++] & 63
            }
            if (u0 < 65536) {
              str += String.fromCharCode(u0)
            } else {
              var ch = u0 - 65536;
              str += String.fromCharCode(55296 | ch >> 10, 56320 | ch & 1023)
            }
          }
        }
        return str
      }

      function UTF8ToString(ptr, maxBytesToRead) {
        return ptr ? UTF8ArrayToString(HEAPU8, ptr, maxBytesToRead) : ""
      }
      var WASM_PAGE_SIZE = 65536;

      function alignUp(x, multiple) {
        if (x % multiple > 0) {
          x += multiple - x % multiple
        }
        return x
      }
      var buffer, HEAP8, HEAPU8, HEAP16, HEAPU16, HEAP32, HEAPU32, HEAPF32, HEAPF64;

      function updateGlobalBufferAndViews(buf) {
        buffer = buf;
        Module["HEAP8"] = HEAP8 = new Int8Array(buf);
        Module["HEAP16"] = HEAP16 = new Int16Array(buf);
        Module["HEAP32"] = HEAP32 = new Int32Array(buf);
        Module["HEAPU8"] = HEAPU8 = new Uint8Array(buf);
        Module["HEAPU16"] = HEAPU16 = new Uint16Array(buf);
        Module["HEAPU32"] = HEAPU32 = new Uint32Array(buf);
        Module["HEAPF32"] = HEAPF32 = new Float32Array(buf);
        Module["HEAPF64"] = HEAPF64 = new Float64Array(buf)
      }
      var INITIAL_INITIAL_MEMORY = Module["INITIAL_MEMORY"] || 16777216;
      if (Module["wasmMemory"]) {
        wasmMemory = Module["wasmMemory"]
      } else {
        wasmMemory = new WebAssembly.Memory({
          "initial": INITIAL_INITIAL_MEMORY / WASM_PAGE_SIZE,
          "maximum": 2147483648 / WASM_PAGE_SIZE
        })
      }
      if (wasmMemory) {
        buffer = wasmMemory.buffer
      }
      INITIAL_INITIAL_MEMORY = buffer.byteLength;
      updateGlobalBufferAndViews(buffer);
      var __ATPRERUN__ = [];
      var __ATINIT__ = [];
      var __ATMAIN__ = [];
      var __ATPOSTRUN__ = [];
      var runtimeInitialized = false;

      function preRun() {
        if (Module["preRun"]) {
          if (typeof Module["preRun"] == "function") Module["preRun"] = [Module["preRun"]];
          while (Module["preRun"].length) {
            addOnPreRun(Module["preRun"].shift())
          }
        }
        callRuntimeCallbacks(__ATPRERUN__)
      }

      function initRuntime() {
        runtimeInitialized = true;
        callRuntimeCallbacks(__ATINIT__)
      }

      function preMain() {
        callRuntimeCallbacks(__ATMAIN__)
      }

      function postRun() {
        if (Module["postRun"]) {
          if (typeof Module["postRun"] == "function") Module["postRun"] = [Module["postRun"]];
          while (Module["postRun"].length) {
            addOnPostRun(Module["postRun"].shift())
          }
        }
        callRuntimeCallbacks(__ATPOSTRUN__)
      }

      function addOnPreRun(cb) {
        __ATPRERUN__.unshift(cb)
      }

      function addOnPostRun(cb) {
        __ATPOSTRUN__.unshift(cb)
      }
      var runDependencies = 0;
      var runDependencyWatcher = null;
      var dependenciesFulfilled = null;

      function addRunDependency(id) {
        runDependencies++;
        if (Module["monitorRunDependencies"]) {
          Module["monitorRunDependencies"](runDependencies)
        }
      }

      function removeRunDependency(id) {
        runDependencies--;
        if (Module["monitorRunDependencies"]) {
          Module["monitorRunDependencies"](runDependencies)
        }
        if (runDependencies == 0) {
          if (runDependencyWatcher !== null) {
            clearInterval(runDependencyWatcher);
            runDependencyWatcher = null
          }
          if (dependenciesFulfilled) {
            var callback = dependenciesFulfilled;
            dependenciesFulfilled = null;
            callback()
          }
        }
      }
      Module["preloadedImages"] = {};
      Module["preloadedAudios"] = {};

      function abort(what) {
        if (Module["onAbort"]) {
          Module["onAbort"](what)
        }
        what += "";
        err(what);
        ABORT = true;
        EXITSTATUS = 1;
        what = "abort(" + what + "). Build with -s ASSERTIONS=1 for more info.";
        var e = new WebAssembly.RuntimeError(what);
        readyPromiseReject(e);
        throw e
      }

      function hasPrefix(str, prefix) {
        return String.prototype.startsWith ? str.startsWith(prefix) : str.indexOf(prefix) === 0
      }
      var dataURIPrefix = "data:application/octet-stream;base64,";

      function isDataURI(filename) {
        return hasPrefix(filename, dataURIPrefix)
      }
      var fileURIPrefix = "file://";

      function isFileURI(filename) {
        return hasPrefix(filename, fileURIPrefix)
      }
      var wasmBinaryFile = "secp256k1.wasm";
      if (!isDataURI(wasmBinaryFile)) {
        wasmBinaryFile = locateFile(wasmBinaryFile)
      }

      function getBinary() {
        try {
          if (wasmBinary) {
            return new Uint8Array(wasmBinary)
          }
          if (readBinary) {
            return readBinary(wasmBinaryFile)
          } else {
            throw "both async and sync fetching of the wasm failed"
          }
        } catch (err) {
          abort(err)
        }
      }

      function getBinaryPromise() {
        if (!wasmBinary && (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) && typeof fetch === "function" && !isFileURI(wasmBinaryFile)) {
          return fetch(wasmBinaryFile, {
            credentials: "same-origin"
          }).then(function(response) {
            if (!response["ok"]) {
              throw "failed to load wasm binary file at '" + wasmBinaryFile + "'"
            }
            return response["arrayBuffer"]()
          }).catch(function() {
            return getBinary()
          })
        }
        return Promise.resolve().then(getBinary)
      }

      function createWasm() {
        var info = {
          "a": asmLibraryArg
        };

        function receiveInstance(instance, module) {
          var exports = instance.exports;
          Module["asm"] = exports;
          wasmTable = Module["asm"]["h"];
          removeRunDependency("wasm-instantiate")
        }
        addRunDependency("wasm-instantiate");

        function receiveInstantiatedSource(output) {
          receiveInstance(output["instance"])
        }

        function instantiateArrayBuffer(receiver) {
          return getBinaryPromise().then(function(binary) {
            return WebAssembly.instantiate(binary, info)
          }).then(receiver, function(reason) {
            err("failed to asynchronously prepare wasm: " + reason);
            abort(reason)
          })
        }

        function instantiateAsync() {
          if (!wasmBinary && typeof WebAssembly.instantiateStreaming === "function" && !isDataURI(wasmBinaryFile) && !isFileURI(wasmBinaryFile) && typeof fetch === "function") {
            fetch(wasmBinaryFile, {
              credentials: "same-origin"
            }).then(function(response) {
              var result = WebAssembly.instantiateStreaming(response, info);
              return result.then(receiveInstantiatedSource, function(reason) {
                err("wasm streaming compile failed: " + reason);
                err("falling back to ArrayBuffer instantiation");
                return instantiateArrayBuffer(receiveInstantiatedSource)
              })
            })
          } else {
            return instantiateArrayBuffer(receiveInstantiatedSource)
          }
        }
        if (Module["instantiateWasm"]) {
          try {
            var exports = Module["instantiateWasm"](info, receiveInstance);
            return exports
          } catch (e) {
            err("Module.instantiateWasm callback failed with error: " + e);
            return false
          }
        }
        instantiateAsync();
        return {}
      }

      function callRuntimeCallbacks(callbacks) {
        while (callbacks.length > 0) {
          var callback = callbacks.shift();
          if (typeof callback == "function") {
            callback(Module);
            continue
          }
          var func = callback.func;
          if (typeof func === "number") {
            if (callback.arg === undefined) {
              wasmTable.get(func)()
            } else {
              wasmTable.get(func)(callback.arg)
            }
          } else {
            func(callback.arg === undefined ? null : callback.arg)
          }
        }
      }

      function _abort() {
        abort()
      }

      function _emscripten_memcpy_big(dest, src, num) {
        HEAPU8.copyWithin(dest, src, src + num)
      }

      function _emscripten_get_heap_size() {
        return HEAPU8.length
      }

      function emscripten_realloc_buffer(size) {
        try {
          wasmMemory.grow(size - buffer.byteLength + 65535 >>> 16);
          updateGlobalBufferAndViews(wasmMemory.buffer);
          return 1
        } catch (e) {}
      }

      function _emscripten_resize_heap(requestedSize) {
        requestedSize = requestedSize >>> 0;
        var oldSize = _emscripten_get_heap_size();
        var maxHeapSize = 2147483648;
        if (requestedSize > maxHeapSize) {
          return false
        }
        var minHeapSize = 16777216;
        for (var cutDown = 1; cutDown <= 4; cutDown *= 2) {
          var overGrownHeapSize = oldSize * (1 + .2 / cutDown);
          overGrownHeapSize = Math.min(overGrownHeapSize, requestedSize + 100663296);
          var newSize = Math.min(maxHeapSize, alignUp(Math.max(minHeapSize, requestedSize, overGrownHeapSize), 65536));
          var replacement = emscripten_realloc_buffer(newSize);
          if (replacement) {
            return true
          }
        }
        return false
      }
      var SYSCALLS = {
        mappings: {},
        buffers: [null, [],
                  []
                 ],
        printChar: function(stream, curr) {
          var buffer = SYSCALLS.buffers[stream];
          if (curr === 0 || curr === 10) {
            (stream === 1 ? out : err)(UTF8ArrayToString(buffer, 0));
            buffer.length = 0
          } else {
            buffer.push(curr)
          }
        },
        varargs: undefined,
        get: function() {
          SYSCALLS.varargs += 4;
          var ret = HEAP32[SYSCALLS.varargs - 4 >> 2];
          return ret
        },
        getStr: function(ptr) {
          var ret = UTF8ToString(ptr);
          return ret
        },
        get64: function(low, high) {
          return low
        }
      };

      function _fd_close(fd) {
        return 0
      }

      function _fd_seek(fd, offset_low, offset_high, whence, newOffset) {}

      function _fd_write(fd, iov, iovcnt, pnum) {
        var num = 0;
        for (var i = 0; i < iovcnt; i++) {
          var ptr = HEAP32[iov + i * 8 >> 2];
          var len = HEAP32[iov + (i * 8 + 4) >> 2];
          for (var j = 0; j < len; j++) {
            SYSCALLS.printChar(fd, HEAPU8[ptr + j])
          }
          num += len
        }
        HEAP32[pnum >> 2] = num;
        return 0
      }
      __ATINIT__.push({
        func: function() {
          ___wasm_call_ctors()
        }
      });
      var asmLibraryArg = {
        "c": _abort,
        "e": _emscripten_memcpy_big,
        "f": _emscripten_resize_heap,
        "g": _fd_close,
        "d": _fd_seek,
        "b": _fd_write,
        "a": wasmMemory
      };
      var asm = createWasm();
      var ___wasm_call_ctors = Module["___wasm_call_ctors"] = function() {
        return (___wasm_call_ctors = Module["___wasm_call_ctors"] = Module["asm"]["i"]).apply(null, arguments)
      };
      var _secp256k1_context_create = Module["_secp256k1_context_create"] = function() {
        return (_secp256k1_context_create = Module["_secp256k1_context_create"] = Module["asm"]["j"]).apply(null, arguments)
      };
      var _free = Module["_free"] = function() {
        return (_free = Module["_free"] = Module["asm"]["k"]).apply(null, arguments)
      };
      var _malloc = Module["_malloc"] = function() {
        return (_malloc = Module["_malloc"] = Module["asm"]["l"]).apply(null, arguments)
      };
      var _secp256k1_ec_pubkey_parse = Module["_secp256k1_ec_pubkey_parse"] = function() {
        return (_secp256k1_ec_pubkey_parse = Module["_secp256k1_ec_pubkey_parse"] = Module["asm"]["m"]).apply(null, arguments)
      };
      var _secp256k1_ec_pubkey_serialize = Module["_secp256k1_ec_pubkey_serialize"] = function() {
        return (_secp256k1_ec_pubkey_serialize = Module["_secp256k1_ec_pubkey_serialize"] = Module["asm"]["n"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_signature_parse_compact = Module["_secp256k1_ecdsa_signature_parse_compact"] = function() {
        return (_secp256k1_ecdsa_signature_parse_compact = Module["_secp256k1_ecdsa_signature_parse_compact"] = Module["asm"]["o"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_signature_serialize_der = Module["_secp256k1_ecdsa_signature_serialize_der"] = function() {
        return (_secp256k1_ecdsa_signature_serialize_der = Module["_secp256k1_ecdsa_signature_serialize_der"] = Module["asm"]["p"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_signature_serialize_compact = Module["_secp256k1_ecdsa_signature_serialize_compact"] = function() {
        return (_secp256k1_ecdsa_signature_serialize_compact = Module["_secp256k1_ecdsa_signature_serialize_compact"] = Module["asm"]["q"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_verify = Module["_secp256k1_ecdsa_verify"] = function() {
        return (_secp256k1_ecdsa_verify = Module["_secp256k1_ecdsa_verify"] = Module["asm"]["r"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_sign = Module["_secp256k1_ecdsa_sign"] = function() {
        return (_secp256k1_ecdsa_sign = Module["_secp256k1_ecdsa_sign"] = Module["asm"]["s"]).apply(null, arguments)
      };
      var _secp256k1_ec_seckey_verify = Module["_secp256k1_ec_seckey_verify"] = function() {
        return (_secp256k1_ec_seckey_verify = Module["_secp256k1_ec_seckey_verify"] = Module["asm"]["t"]).apply(null, arguments)
      };
      var _secp256k1_ec_pubkey_create = Module["_secp256k1_ec_pubkey_create"] = function() {
        return (_secp256k1_ec_pubkey_create = Module["_secp256k1_ec_pubkey_create"] = Module["asm"]["u"]).apply(null, arguments)
      };
      var _secp256k1_context_randomize = Module["_secp256k1_context_randomize"] = function() {
        return (_secp256k1_context_randomize = Module["_secp256k1_context_randomize"] = Module["asm"]["v"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_recoverable_signature_parse_compact = Module["_secp256k1_ecdsa_recoverable_signature_parse_compact"] = function() {
        return (_secp256k1_ecdsa_recoverable_signature_parse_compact = Module["_secp256k1_ecdsa_recoverable_signature_parse_compact"] = Module["asm"]["w"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_recoverable_signature_serialize_compact = Module["_secp256k1_ecdsa_recoverable_signature_serialize_compact"] = function() {
        return (_secp256k1_ecdsa_recoverable_signature_serialize_compact = Module["_secp256k1_ecdsa_recoverable_signature_serialize_compact"] = Module["asm"]["x"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_recoverable_signature_convert = Module["_secp256k1_ecdsa_recoverable_signature_convert"] = function() {
        return (_secp256k1_ecdsa_recoverable_signature_convert = Module["_secp256k1_ecdsa_recoverable_signature_convert"] = Module["asm"]["y"]).apply(null, arguments)
      };
      var _secp256k1_ecdsa_sign_recoverable = Module["_secp256k1_ecdsa_sign_recoverable"] = function() {
        return (_secp256k1_ecdsa_sign_recoverable = Module["_secp256k1_ecdsa_sign_recoverable"] = Module["asm"]["z"]).apply(null, arguments)
      };
      var _ml_secp256k1_fe_const_bytecode = Module["_ml_secp256k1_fe_const_bytecode"] = function() {
        return (_ml_secp256k1_fe_const_bytecode = Module["_ml_secp256k1_fe_const_bytecode"] = Module["asm"]["x"]).apply(null, arguments)
      };
      var _ml_secp256k1_fe_set_b32 = Module["_ml_secp256k1_fe_set_b32"] = function() {
        return (_ml_secp256k1_fe_set_b32 = Module["_ml_secp256k1_fe_set_b32"] = Module["asm"]["y"]).apply(null, arguments)
      };
      var _ml_secp256k1_ge_of_fields = Module["_ml_secp256k1_ge_of_fields"] = function() {
        return (_ml_secp256k1_ge_of_fields = Module["_ml_secp256k1_ge_of_fields"] = Module["asm"]["z"]).apply(null, arguments)
      };
      var _ml_secp256k1_gej_of_fields = Module["_ml_secp256k1_gej_of_fields"] = function() {
        return (_ml_secp256k1_gej_of_fields = Module["_ml_secp256k1_gej_of_fields"] = Module["asm"]["A"]).apply(null, arguments)
      };
      var _ml_secp256k1_gej_set_ge = Module["_ml_secp256k1_gej_set_ge"] = function() {
        return (_ml_secp256k1_gej_set_ge = Module["_ml_secp256k1_gej_set_ge"] = Module["asm"]["B"]).apply(null, arguments)
      };
      var calledRun;

      function ExitStatus(status) {
        this.name = "ExitStatus";
        this.message = "Program terminated with exit(" + status + ")";
        this.status = status
      }
      dependenciesFulfilled = function runCaller() {
        if (!calledRun) run();
        if (!calledRun) dependenciesFulfilled = runCaller
      };

      function run(args) {
        args = args || arguments_;
        if (runDependencies > 0) {
          return
        }
        preRun();
        if (runDependencies > 0) return;

        function doRun() {
          if (calledRun) return;
          calledRun = true;
          Module["calledRun"] = true;
          if (ABORT) return;
          initRuntime();
          preMain();
          readyPromiseResolve(Module);
          if (Module["onRuntimeInitialized"]) Module["onRuntimeInitialized"]();
          postRun()
        }
        if (Module["setStatus"]) {
          Module["setStatus"]("Running...");
          setTimeout(function() {
            setTimeout(function() {
              Module["setStatus"]("")
            }, 1);
            doRun()
          }, 1)
        } else {
          doRun()
        }
      }
      Module["run"] = run;
      if (Module["preInit"]) {
        if (typeof Module["preInit"] == "function") Module["preInit"] = [Module["preInit"]];
        while (Module["preInit"].length > 0) {
          Module["preInit"].pop()()
        }
      }
      noExitRuntime = true;
      run();


      return _SECP256K1.ready
    }
  );
})();
if (typeof exports === 'object' && typeof module === 'object')
  module.exports = _SECP256K1;
else if (typeof define === 'function' && define['amd'])
  define([], function() {
    return _SECP256K1;
  });
else if (typeof exports === 'object')
  exports["_SECP256K1"] = _SECP256K1;
