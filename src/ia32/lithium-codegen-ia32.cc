// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#if V8_TARGET_ARCH_IA32

#include "src/base/bits.h"
#include "src/code-factory.h"
#include "src/code-stubs.h"
#include "src/codegen.h"
#include "src/cpu-profiler.h"
#include "src/deoptimizer.h"
#include "src/hydrogen-osr.h"
#include "src/ia32/lithium-codegen-ia32.h"
#include "src/ic/ic.h"
#include "src/ic/stub-cache.h"

namespace v8 {
namespace internal {

inline bool IsSIMD128LoadStoreOp(BuiltinFunctionId op) {
  return (op == kFloat32ArrayGetFloat32x4XYZW ||
      op == kFloat32ArrayGetFloat32x4X ||
      op == kFloat32ArrayGetFloat32x4XY ||
      op == kFloat32ArrayGetFloat32x4XYZ ||
      op == kFloat64ArrayGetFloat64x2XY ||
      op == kFloat64ArrayGetFloat64x2X ||
      op == kInt32ArrayGetInt32x4XYZW ||
      op == kInt32ArrayGetInt32x4X ||
      op == kInt32ArrayGetInt32x4XY ||
      op == kInt32ArrayGetInt32x4XYZ ||
      op == kInt8ArrayGetFloat32x4XYZW ||
      op == kInt8ArrayGetFloat32x4X ||
      op == kInt8ArrayGetFloat32x4XY ||
      op == kInt8ArrayGetFloat32x4XYZ ||
      op == kInt8ArrayGetFloat64x2XY ||
      op == kInt8ArrayGetFloat64x2X ||
      op == kInt8ArrayGetInt32x4XYZW ||
      op == kInt8ArrayGetInt32x4X ||
      op == kInt8ArrayGetInt32x4XY ||
      op == kInt8ArrayGetInt32x4XYZ ||
      op == kUint8ArrayGetFloat32x4XYZW ||
      op == kUint8ArrayGetFloat32x4X ||
      op == kUint8ArrayGetFloat32x4XY ||
      op == kUint8ArrayGetFloat32x4XYZ ||
      op == kUint8ArrayGetFloat64x2XY ||
      op == kUint8ArrayGetFloat64x2X ||
      op == kUint8ArrayGetInt32x4XYZW ||
      op == kUint8ArrayGetInt32x4X ||
      op == kUint8ArrayGetInt32x4XY ||
      op == kUint8ArrayGetInt32x4XYZ ||
      op == kFloat32ArraySetFloat32x4XYZW ||
      op == kFloat32ArraySetFloat32x4X ||
      op == kFloat32ArraySetFloat32x4XY ||
      op == kFloat32ArraySetFloat32x4XYZ ||
      op == kFloat64ArraySetFloat64x2XY ||
      op == kFloat64ArraySetFloat64x2X ||
      op == kInt32ArraySetInt32x4XYZW ||
      op == kInt32ArraySetInt32x4X ||
      op == kInt32ArraySetInt32x4XY ||
      op == kInt32ArraySetInt32x4XYZ ||
      op == kInt8ArraySetFloat32x4XYZW ||
      op == kInt8ArraySetFloat32x4X ||
      op == kInt8ArraySetFloat32x4XY ||
      op == kInt8ArraySetFloat32x4XYZ ||
      op == kInt8ArraySetFloat64x2XY ||
      op == kInt8ArraySetFloat64x2X ||
      op == kInt8ArraySetInt32x4XYZW ||
      op == kInt8ArraySetInt32x4X ||
      op == kInt8ArraySetInt32x4XY ||
      op == kInt8ArraySetInt32x4XYZ ||
      op == kUint8ArraySetFloat32x4XYZW ||
      op == kUint8ArraySetFloat32x4X ||
      op == kUint8ArraySetFloat32x4XY ||
      op == kUint8ArraySetFloat32x4XYZ ||
      op == kUint8ArraySetFloat64x2XY ||
      op == kUint8ArraySetFloat64x2X ||
      op == kUint8ArraySetInt32x4XYZW ||
      op == kUint8ArraySetInt32x4X ||
      op == kUint8ArraySetInt32x4XY ||
      op == kUint8ArraySetInt32x4XYZ);
}


int GetSIMD128LoadStoreBytes(BuiltinFunctionId op) {
  if (op == kFloat32ArrayGetFloat32x4XYZW ||
      op == kFloat64ArrayGetFloat64x2XY ||
      op == kInt32ArrayGetInt32x4XYZW ||
      op == kInt8ArrayGetFloat32x4XYZW ||
      op == kInt8ArrayGetFloat64x2XY ||
      op == kInt8ArrayGetInt32x4XYZW ||
      op == kUint8ArrayGetFloat32x4XYZW ||
      op == kUint8ArrayGetFloat64x2XY ||
      op == kUint8ArrayGetInt32x4XYZW ||
      op == kFloat32ArraySetFloat32x4XYZW ||
      op == kFloat64ArraySetFloat64x2XY ||
      op == kInt32ArraySetInt32x4XYZW ||
      op == kInt8ArraySetFloat32x4XYZW ||
      op == kInt8ArraySetFloat64x2XY ||
      op == kInt8ArraySetInt32x4XYZW ||
      op == kUint8ArraySetFloat32x4XYZW ||
      op == kUint8ArraySetFloat64x2XY ||
      op == kUint8ArraySetInt32x4XYZW) {
    return 16;
  } else if (op == kFloat32ArrayGetFloat32x4X ||
      op == kInt32ArrayGetInt32x4X ||
      op == kInt8ArrayGetFloat32x4X ||
      op == kInt8ArrayGetInt32x4X ||
      op == kUint8ArrayGetFloat32x4X ||
      op == kUint8ArrayGetInt32x4X ||
      op == kFloat32ArraySetFloat32x4X ||
      op == kInt32ArraySetInt32x4X ||
      op == kInt8ArraySetFloat32x4X ||
      op == kInt8ArraySetInt32x4X ||
      op == kUint8ArraySetFloat32x4X ||
      op == kUint8ArraySetInt32x4X) {
    return 4;
  } else if (op == kFloat32ArrayGetFloat32x4XY ||
      op == kFloat64ArrayGetFloat64x2X ||
      op == kInt32ArrayGetInt32x4XY ||
      op == kInt8ArrayGetFloat32x4XY ||
      op == kInt8ArrayGetFloat64x2X ||
      op == kInt8ArrayGetInt32x4XY ||
      op == kUint8ArrayGetFloat32x4XY ||
      op == kUint8ArrayGetFloat64x2X ||
      op == kUint8ArrayGetInt32x4XY ||
      op == kFloat32ArraySetFloat32x4XY ||
      op == kFloat64ArraySetFloat64x2X ||
      op == kInt32ArraySetInt32x4XY ||
      op == kInt8ArraySetFloat32x4XY ||
      op == kInt8ArraySetFloat64x2X ||
      op == kInt8ArraySetInt32x4XY ||
      op == kUint8ArraySetFloat32x4XY ||
      op == kUint8ArraySetFloat64x2X ||
      op == kUint8ArraySetInt32x4XY) {
    return 8;
  } else if (op == kFloat32ArrayGetFloat32x4XYZ ||
      op == kInt32ArrayGetInt32x4XYZ ||
      op == kInt8ArrayGetFloat32x4XYZ ||
      op == kInt8ArrayGetInt32x4XYZ ||
      op == kUint8ArrayGetFloat32x4XYZ ||
      op == kUint8ArrayGetInt32x4XYZ ||
      op == kFloat32ArraySetFloat32x4XYZ ||
      op == kInt32ArraySetInt32x4XYZ ||
      op == kInt8ArraySetFloat32x4XYZ ||
      op == kInt8ArraySetInt32x4XYZ ||
      op == kUint8ArraySetFloat32x4XYZ ||
      op == kUint8ArraySetInt32x4XYZ) {
    return 12;
  } else {
    UNREACHABLE();
    return -1;
  }
}


// When invoking builtins, we need to record the safepoint in the middle of
// the invoke instruction sequence generated by the macro assembler.
class SafepointGenerator final : public CallWrapper {
 public:
  SafepointGenerator(LCodeGen* codegen,
                     LPointerMap* pointers,
                     Safepoint::DeoptMode mode)
      : codegen_(codegen),
        pointers_(pointers),
        deopt_mode_(mode) {}
  virtual ~SafepointGenerator() {}

  void BeforeCall(int call_size) const override {}

  void AfterCall() const override {
    codegen_->RecordSafepoint(pointers_, deopt_mode_);
  }

 private:
  LCodeGen* codegen_;
  LPointerMap* pointers_;
  Safepoint::DeoptMode deopt_mode_;
};


#define __ masm()->

bool LCodeGen::GenerateCode() {
  LPhase phase("Z_Code generation", chunk());
  DCHECK(is_unused());
  status_ = GENERATING;

  // Open a frame scope to indicate that there is a frame on the stack.  The
  // MANUAL indicates that the scope shouldn't actually generate code to set up
  // the frame (that is done in GeneratePrologue).
  FrameScope frame_scope(masm_, StackFrame::MANUAL);

  support_aligned_spilled_doubles_ = info()->IsOptimizing();

  dynamic_frame_alignment_ = info()->IsOptimizing() &&
      ((chunk()->num_double_slots() > 2 &&
        !chunk()->graph()->is_recursive()) ||
       !info()->osr_ast_id().IsNone());

  return GeneratePrologue() &&
      GenerateBody() &&
      GenerateDeferredCode() &&
      GenerateJumpTable() &&
      GenerateSafepointTable();
}


void LCodeGen::FinishCode(Handle<Code> code) {
  DCHECK(is_done());
  code->set_stack_slots(GetStackSlotCount());
  code->set_safepoint_table_offset(safepoints_.GetCodeOffset());
  PopulateDeoptimizationData(code);
  if (!info()->IsStub()) {
    Deoptimizer::EnsureRelocSpaceForLazyDeoptimization(code);
  }
}


#ifdef _MSC_VER
void LCodeGen::MakeSureStackPagesMapped(int offset) {
  const int kPageSize = 4 * KB;
  for (offset -= kPageSize; offset > 0; offset -= kPageSize) {
    __ mov(Operand(esp, offset), eax);
  }
}
#endif


void LCodeGen::SaveCallerDoubles() {
  DCHECK(info()->saves_caller_doubles());
  DCHECK(NeedsEagerFrame());
  Comment(";;; Save clobbered callee double registers");
  int count = 0;
  BitVector* doubles = chunk()->allocated_double_registers();
  BitVector::Iterator save_iterator(doubles);
  while (!save_iterator.Done()) {
    __ movsd(MemOperand(esp, count * kDoubleSize),
              XMMRegister::FromAllocationIndex(save_iterator.Current()));
    save_iterator.Advance();
    count++;
  }
}


void LCodeGen::RestoreCallerDoubles() {
  DCHECK(info()->saves_caller_doubles());
  DCHECK(NeedsEagerFrame());
  Comment(";;; Restore clobbered callee double registers");
  BitVector* doubles = chunk()->allocated_double_registers();
  BitVector::Iterator save_iterator(doubles);
  int count = 0;
  while (!save_iterator.Done()) {
    __ movsd(XMMRegister::FromAllocationIndex(save_iterator.Current()),
              MemOperand(esp, count * kDoubleSize));
    save_iterator.Advance();
    count++;
  }
}


bool LCodeGen::GeneratePrologue() {
  DCHECK(is_generating());

  if (info()->IsOptimizing()) {
    ProfileEntryHookStub::MaybeCallEntryHook(masm_);

#ifdef DEBUG
    if (strlen(FLAG_stop_at) > 0 &&
        info_->function()->name()->IsUtf8EqualTo(CStrVector(FLAG_stop_at))) {
      __ int3();
    }
#endif

    // Sloppy mode functions and builtins need to replace the receiver with the
    // global proxy when called as functions (without an explicit receiver
    // object).
    if (is_sloppy(info()->language_mode()) && info()->MayUseThis() &&
        !info()->is_native() && info()->scope()->has_this_declaration()) {
      Label ok;
      // +1 for return address.
      int receiver_offset = (scope()->num_parameters() + 1) * kPointerSize;
      __ mov(ecx, Operand(esp, receiver_offset));

      __ cmp(ecx, isolate()->factory()->undefined_value());
      __ j(not_equal, &ok, Label::kNear);

      __ mov(ecx, GlobalObjectOperand());
      __ mov(ecx, FieldOperand(ecx, GlobalObject::kGlobalProxyOffset));

      __ mov(Operand(esp, receiver_offset), ecx);

      __ bind(&ok);
    }

    if (support_aligned_spilled_doubles_ && dynamic_frame_alignment_) {
      // Move state of dynamic frame alignment into edx.
      __ Move(edx, Immediate(kNoAlignmentPadding));

      Label do_not_pad, align_loop;
      STATIC_ASSERT(kDoubleSize == 2 * kPointerSize);
      // Align esp + 4 to a multiple of 2 * kPointerSize.
      __ test(esp, Immediate(kPointerSize));
      __ j(not_zero, &do_not_pad, Label::kNear);
      __ push(Immediate(0));
      __ mov(ebx, esp);
      __ mov(edx, Immediate(kAlignmentPaddingPushed));
      // Copy arguments, receiver, and return address.
      __ mov(ecx, Immediate(scope()->num_parameters() + 2));

      __ bind(&align_loop);
      __ mov(eax, Operand(ebx, 1 * kPointerSize));
      __ mov(Operand(ebx, 0), eax);
      __ add(Operand(ebx), Immediate(kPointerSize));
      __ dec(ecx);
      __ j(not_zero, &align_loop, Label::kNear);
      __ mov(Operand(ebx, 0), Immediate(kAlignmentZapValue));
      __ bind(&do_not_pad);
    }
  }

  info()->set_prologue_offset(masm_->pc_offset());
  if (NeedsEagerFrame()) {
    DCHECK(!frame_is_built_);
    frame_is_built_ = true;
    if (info()->IsStub()) {
      __ StubPrologue();
    } else {
      __ Prologue(info()->IsCodePreAgingActive());
    }
    info()->AddNoFrameRange(0, masm_->pc_offset());
  }

  if (info()->IsOptimizing() &&
      dynamic_frame_alignment_ &&
      FLAG_debug_code) {
    __ test(esp, Immediate(kPointerSize));
    __ Assert(zero, kFrameIsExpectedToBeAligned);
  }

  // Reserve space for the stack slots needed by the code.
  int slots = GetStackSlotCount();
  DCHECK(slots != 0 || !info()->IsOptimizing());
  if (slots > 0) {
    if (slots == 1) {
      if (dynamic_frame_alignment_) {
        __ push(edx);
      } else {
        __ push(Immediate(kNoAlignmentPadding));
      }
    } else {
      if (FLAG_debug_code) {
        __ sub(Operand(esp), Immediate(slots * kPointerSize));
#ifdef _MSC_VER
        MakeSureStackPagesMapped(slots * kPointerSize);
#endif
        __ push(eax);
        __ mov(Operand(eax), Immediate(slots));
        Label loop;
        __ bind(&loop);
        __ mov(MemOperand(esp, eax, times_4, 0),
               Immediate(kSlotsZapValue));
        __ dec(eax);
        __ j(not_zero, &loop);
        __ pop(eax);
      } else {
        __ sub(Operand(esp), Immediate(slots * kPointerSize));
#ifdef _MSC_VER
        MakeSureStackPagesMapped(slots * kPointerSize);
#endif
      }

      if (support_aligned_spilled_doubles_) {
        Comment(";;; Store dynamic frame alignment tag for spilled doubles");
        // Store dynamic frame alignment state in the first local.
        int offset = JavaScriptFrameConstants::kDynamicAlignmentStateOffset;
        if (dynamic_frame_alignment_) {
          __ mov(Operand(ebp, offset), edx);
        } else {
          __ mov(Operand(ebp, offset), Immediate(kNoAlignmentPadding));
        }
      }
    }

    if (info()->saves_caller_doubles()) SaveCallerDoubles();
  }

  // Possibly allocate a local context.
  int heap_slots = info_->num_heap_slots() - Context::MIN_CONTEXT_SLOTS;
  if (heap_slots > 0) {
    Comment(";;; Allocate local context");
    bool need_write_barrier = true;
    // Argument to NewContext is the function, which is still in edi.
    DCHECK(!info()->scope()->is_script_scope());
    if (heap_slots <= FastNewContextStub::kMaximumSlots) {
      FastNewContextStub stub(isolate(), heap_slots);
      __ CallStub(&stub);
      // Result of FastNewContextStub is always in new space.
      need_write_barrier = false;
    } else {
      __ push(edi);
      __ CallRuntime(Runtime::kNewFunctionContext, 1);
    }
    RecordSafepoint(Safepoint::kNoLazyDeopt);
    // Context is returned in eax.  It replaces the context passed to us.
    // It's saved in the stack and kept live in esi.
    __ mov(esi, eax);
    __ mov(Operand(ebp, StandardFrameConstants::kContextOffset), eax);

    // Copy parameters into context if necessary.
    int num_parameters = scope()->num_parameters();
    int first_parameter = scope()->has_this_declaration() ? -1 : 0;
    for (int i = first_parameter; i < num_parameters; i++) {
      Variable* var = (i == -1) ? scope()->receiver() : scope()->parameter(i);
      if (var->IsContextSlot()) {
        int parameter_offset = StandardFrameConstants::kCallerSPOffset +
            (num_parameters - 1 - i) * kPointerSize;
        // Load parameter from stack.
        __ mov(eax, Operand(ebp, parameter_offset));
        // Store it in the context.
        int context_offset = Context::SlotOffset(var->index());
        __ mov(Operand(esi, context_offset), eax);
        // Update the write barrier. This clobbers eax and ebx.
        if (need_write_barrier) {
          __ RecordWriteContextSlot(esi,
                                    context_offset,
                                    eax,
                                    ebx,
                                    kDontSaveFPRegs);
        } else if (FLAG_debug_code) {
          Label done;
          __ JumpIfInNewSpace(esi, eax, &done, Label::kNear);
          __ Abort(kExpectedNewSpaceObject);
          __ bind(&done);
        }
      }
    }
    Comment(";;; End allocate local context");
  }

  // Trace the call.
  if (FLAG_trace && info()->IsOptimizing()) {
    // We have not executed any compiled code yet, so esi still holds the
    // incoming context.
    __ CallRuntime(Runtime::kTraceEnter, 0);
  }
  return !is_aborted();
}


void LCodeGen::GenerateOsrPrologue() {
  // Generate the OSR entry prologue at the first unknown OSR value, or if there
  // are none, at the OSR entrypoint instruction.
  if (osr_pc_offset_ >= 0) return;

  osr_pc_offset_ = masm()->pc_offset();

    // Move state of dynamic frame alignment into edx.
  __ Move(edx, Immediate(kNoAlignmentPadding));

  if (support_aligned_spilled_doubles_ && dynamic_frame_alignment_) {
    Label do_not_pad, align_loop;
    // Align ebp + 4 to a multiple of 2 * kPointerSize.
    __ test(ebp, Immediate(kPointerSize));
    __ j(zero, &do_not_pad, Label::kNear);
    __ push(Immediate(0));
    __ mov(ebx, esp);
    __ mov(edx, Immediate(kAlignmentPaddingPushed));

    // Move all parts of the frame over one word. The frame consists of:
    // unoptimized frame slots, alignment state, context, frame pointer, return
    // address, receiver, and the arguments.
    __ mov(ecx, Immediate(scope()->num_parameters() +
           5 + graph()->osr()->UnoptimizedFrameSlots()));

    __ bind(&align_loop);
    __ mov(eax, Operand(ebx, 1 * kPointerSize));
    __ mov(Operand(ebx, 0), eax);
    __ add(Operand(ebx), Immediate(kPointerSize));
    __ dec(ecx);
    __ j(not_zero, &align_loop, Label::kNear);
    __ mov(Operand(ebx, 0), Immediate(kAlignmentZapValue));
    __ sub(Operand(ebp), Immediate(kPointerSize));
    __ bind(&do_not_pad);
  }

  // Save the first local, which is overwritten by the alignment state.
  Operand alignment_loc = MemOperand(ebp, -3 * kPointerSize);
  __ push(alignment_loc);

  // Set the dynamic frame alignment state.
  __ mov(alignment_loc, edx);

  // Adjust the frame size, subsuming the unoptimized frame into the
  // optimized frame.
  int slots = GetStackSlotCount() - graph()->osr()->UnoptimizedFrameSlots();
  DCHECK(slots >= 1);
  __ sub(esp, Immediate((slots - 1) * kPointerSize));
}


void LCodeGen::GenerateBodyInstructionPre(LInstruction* instr) {
  if (instr->IsCall()) {
    EnsureSpaceForLazyDeopt(Deoptimizer::patch_size());
  }
  if (!instr->IsLazyBailout() && !instr->IsGap()) {
    safepoints_.BumpLastLazySafepointIndex();
  }
}


void LCodeGen::GenerateBodyInstructionPost(LInstruction* instr) { }


bool LCodeGen::GenerateJumpTable() {
  if (!jump_table_.length()) return !is_aborted();

  Label needs_frame;
  Comment(";;; -------------------- Jump table --------------------");

  for (int i = 0; i < jump_table_.length(); i++) {
    Deoptimizer::JumpTableEntry* table_entry = &jump_table_[i];
    __ bind(&table_entry->label);
    Address entry = table_entry->address;
    DeoptComment(table_entry->deopt_info);
    if (table_entry->needs_frame) {
      DCHECK(!info()->saves_caller_doubles());
      __ push(Immediate(ExternalReference::ForDeoptEntry(entry)));
      __ call(&needs_frame);
    } else {
      if (info()->saves_caller_doubles()) RestoreCallerDoubles();
      __ call(entry, RelocInfo::RUNTIME_ENTRY);
    }
    info()->LogDeoptCallPosition(masm()->pc_offset(),
                                 table_entry->deopt_info.inlining_id);
  }
  if (needs_frame.is_linked()) {
    __ bind(&needs_frame);
    /* stack layout
       4: entry address
       3: return address  <-- esp
       2: garbage
       1: garbage
       0: garbage
    */
    __ sub(esp, Immediate(kPointerSize));    // Reserve space for stub marker.
    __ push(MemOperand(esp, kPointerSize));  // Copy return address.
    __ push(MemOperand(esp, 3 * kPointerSize));  // Copy entry address.

    /* stack layout
       4: entry address
       3: return address
       2: garbage
       1: return address
       0: entry address  <-- esp
    */
    __ mov(MemOperand(esp, 4 * kPointerSize), ebp);  // Save ebp.
    // Copy context.
    __ mov(ebp, MemOperand(ebp, StandardFrameConstants::kContextOffset));
    __ mov(MemOperand(esp, 3 * kPointerSize), ebp);
    // Fill ebp with the right stack frame address.
    __ lea(ebp, MemOperand(esp, 4 * kPointerSize));
    // This variant of deopt can only be used with stubs. Since we don't
    // have a function pointer to install in the stack frame that we're
    // building, install a special marker there instead.
    DCHECK(info()->IsStub());
    __ mov(MemOperand(esp, 2 * kPointerSize),
           Immediate(Smi::FromInt(StackFrame::STUB)));

    /* stack layout
       4: old ebp
       3: context pointer
       2: stub marker
       1: return address
       0: entry address  <-- esp
    */
    __ ret(0);  // Call the continuation without clobbering registers.
  }
  return !is_aborted();
}


bool LCodeGen::GenerateDeferredCode() {
  DCHECK(is_generating());
  if (deferred_.length() > 0) {
    for (int i = 0; !is_aborted() && i < deferred_.length(); i++) {
      LDeferredCode* code = deferred_[i];

      HValue* value =
          instructions_->at(code->instruction_index())->hydrogen_value();
      RecordAndWritePosition(
          chunk()->graph()->SourcePositionToScriptPosition(value->position()));

      Comment(";;; <@%d,#%d> "
              "-------------------- Deferred %s --------------------",
              code->instruction_index(),
              code->instr()->hydrogen_value()->id(),
              code->instr()->Mnemonic());
      __ bind(code->entry());
      if (NeedsDeferredFrame()) {
        Comment(";;; Build frame");
        DCHECK(!frame_is_built_);
        DCHECK(info()->IsStub());
        frame_is_built_ = true;
        // Build the frame in such a way that esi isn't trashed.
        __ push(ebp);  // Caller's frame pointer.
        __ push(Operand(ebp, StandardFrameConstants::kContextOffset));
        __ push(Immediate(Smi::FromInt(StackFrame::STUB)));
        __ lea(ebp, Operand(esp, 2 * kPointerSize));
        Comment(";;; Deferred code");
      }
      code->Generate();
      if (NeedsDeferredFrame()) {
        __ bind(code->done());
        Comment(";;; Destroy frame");
        DCHECK(frame_is_built_);
        frame_is_built_ = false;
        __ mov(esp, ebp);
        __ pop(ebp);
      }
      __ jmp(code->exit());
    }
  }

  // Deferred code is the last part of the instruction sequence. Mark
  // the generated code as done unless we bailed out.
  if (!is_aborted()) status_ = DONE;
  return !is_aborted();
}


bool LCodeGen::GenerateSafepointTable() {
  DCHECK(is_done());
  if (!info()->IsStub()) {
    // For lazy deoptimization we need space to patch a call after every call.
    // Ensure there is always space for such patching, even if the code ends
    // in a call.
    int target_offset = masm()->pc_offset() + Deoptimizer::patch_size();
    while (masm()->pc_offset() < target_offset) {
      masm()->nop();
    }
  }
  safepoints_.Emit(masm(), GetStackSlotCount());
  return !is_aborted();
}


Register LCodeGen::ToRegister(int index) const {
  return Register::FromAllocationIndex(index);
}


XMMRegister LCodeGen::ToDoubleRegister(int index) const {
  return XMMRegister::FromAllocationIndex(index);
}


XMMRegister LCodeGen::ToSIMD128Register(int index) const {
  return XMMRegister::FromAllocationIndex(index);
}


Register LCodeGen::ToRegister(LOperand* op) const {
  DCHECK(op->IsRegister());
  return ToRegister(op->index());
}


XMMRegister LCodeGen::ToDoubleRegister(LOperand* op) const {
  DCHECK(op->IsDoubleRegister());
  return ToDoubleRegister(op->index());
}


XMMRegister LCodeGen::ToFloat32x4Register(LOperand* op) const {
  DCHECK(op->IsFloat32x4Register());
  return ToSIMD128Register(op->index());
}


XMMRegister LCodeGen::ToFloat64x2Register(LOperand* op) const {
  DCHECK(op->IsFloat64x2Register());
  return ToSIMD128Register(op->index());
}


XMMRegister LCodeGen::ToInt32x4Register(LOperand* op) const {
  DCHECK(op->IsInt32x4Register());
  return ToSIMD128Register(op->index());
}


XMMRegister LCodeGen::ToSIMD128Register(LOperand* op) const {
  DCHECK(op->IsFloat32x4Register() || op->IsFloat64x2Register() ||
         op->IsInt32x4Register());
  return ToSIMD128Register(op->index());
}


int32_t LCodeGen::ToInteger32(LConstantOperand* op) const {
  return ToRepresentation(op, Representation::Integer32());
}


int32_t LCodeGen::ToRepresentation(LConstantOperand* op,
                                   const Representation& r) const {
  HConstant* constant = chunk_->LookupConstant(op);
  int32_t value = constant->Integer32Value();
  if (r.IsInteger32()) return value;
  DCHECK(r.IsSmiOrTagged());
  return reinterpret_cast<int32_t>(Smi::FromInt(value));
}


Handle<Object> LCodeGen::ToHandle(LConstantOperand* op) const {
  HConstant* constant = chunk_->LookupConstant(op);
  DCHECK(chunk_->LookupLiteralRepresentation(op).IsSmiOrTagged());
  return constant->handle(isolate());
}


double LCodeGen::ToDouble(LConstantOperand* op) const {
  HConstant* constant = chunk_->LookupConstant(op);
  DCHECK(constant->HasDoubleValue());
  return constant->DoubleValue();
}


ExternalReference LCodeGen::ToExternalReference(LConstantOperand* op) const {
  HConstant* constant = chunk_->LookupConstant(op);
  DCHECK(constant->HasExternalReferenceValue());
  return constant->ExternalReferenceValue();
}


bool LCodeGen::IsInteger32(LConstantOperand* op) const {
  return chunk_->LookupLiteralRepresentation(op).IsSmiOrInteger32();
}


bool LCodeGen::IsSmi(LConstantOperand* op) const {
  return chunk_->LookupLiteralRepresentation(op).IsSmi();
}


static int ArgumentsOffsetWithoutFrame(int index) {
  DCHECK(index < 0);
  return -(index + 1) * kPointerSize + kPCOnStackSize;
}


Operand LCodeGen::ToOperand(LOperand* op) const {
  if (op->IsRegister()) return Operand(ToRegister(op));
  if (op->IsDoubleRegister()) return Operand(ToDoubleRegister(op));
  if (op->IsFloat32x4Register()) return Operand(ToFloat32x4Register(op));
  if (op->IsFloat64x2Register()) return Operand(ToFloat64x2Register(op));
  if (op->IsInt32x4Register()) return Operand(ToInt32x4Register(op));
  DCHECK(op->IsStackSlot() || op->IsDoubleStackSlot() ||
         op->IsFloat32x4StackSlot() || op->IsFloat64x2StackSlot() ||
         op->IsInt32x4StackSlot());
  if (NeedsEagerFrame()) {
    return Operand(ebp, StackSlotOffset(op->index()));
  } else {
    // Retrieve parameter without eager stack-frame relative to the
    // stack-pointer.
    return Operand(esp, ArgumentsOffsetWithoutFrame(op->index()));
  }
}


Operand LCodeGen::HighOperand(LOperand* op) {
  DCHECK(op->IsDoubleStackSlot());
  if (NeedsEagerFrame()) {
    return Operand(ebp, StackSlotOffset(op->index()) + kPointerSize);
  } else {
    // Retrieve parameter without eager stack-frame relative to the
    // stack-pointer.
    return Operand(
        esp, ArgumentsOffsetWithoutFrame(op->index()) + kPointerSize);
  }
}


void LCodeGen::WriteTranslation(LEnvironment* environment,
                                Translation* translation) {
  if (environment == NULL) return;

  // The translation includes one command per value in the environment.
  int translation_size = environment->translation_size();

  WriteTranslation(environment->outer(), translation);
  WriteTranslationFrame(environment, translation);

  int object_index = 0;
  int dematerialized_index = 0;
  for (int i = 0; i < translation_size; ++i) {
    LOperand* value = environment->values()->at(i);
    AddToTranslation(
        environment, translation, value, environment->HasTaggedValueAt(i),
        environment->HasUint32ValueAt(i), &object_index, &dematerialized_index);
  }
}


void LCodeGen::AddToTranslation(LEnvironment* environment,
                                Translation* translation,
                                LOperand* op,
                                bool is_tagged,
                                bool is_uint32,
                                int* object_index_pointer,
                                int* dematerialized_index_pointer) {
  if (op == LEnvironment::materialization_marker()) {
    int object_index = (*object_index_pointer)++;
    if (environment->ObjectIsDuplicateAt(object_index)) {
      int dupe_of = environment->ObjectDuplicateOfAt(object_index);
      translation->DuplicateObject(dupe_of);
      return;
    }
    int object_length = environment->ObjectLengthAt(object_index);
    if (environment->ObjectIsArgumentsAt(object_index)) {
      translation->BeginArgumentsObject(object_length);
    } else {
      translation->BeginCapturedObject(object_length);
    }
    int dematerialized_index = *dematerialized_index_pointer;
    int env_offset = environment->translation_size() + dematerialized_index;
    *dematerialized_index_pointer += object_length;
    for (int i = 0; i < object_length; ++i) {
      LOperand* value = environment->values()->at(env_offset + i);
      AddToTranslation(environment,
                       translation,
                       value,
                       environment->HasTaggedValueAt(env_offset + i),
                       environment->HasUint32ValueAt(env_offset + i),
                       object_index_pointer,
                       dematerialized_index_pointer);
    }
    return;
  }

  if (op->IsStackSlot()) {
    if (is_tagged) {
      translation->StoreStackSlot(op->index());
    } else if (is_uint32) {
      translation->StoreUint32StackSlot(op->index());
    } else {
      translation->StoreInt32StackSlot(op->index());
    }
  } else if (op->IsDoubleStackSlot()) {
    translation->StoreDoubleStackSlot(op->index());
  } else if (op->IsFloat32x4StackSlot()) {
    translation->StoreSIMD128StackSlot(op->index(),
                                       Translation::FLOAT32x4_STACK_SLOT);
  } else if (op->IsFloat64x2StackSlot()) {
    translation->StoreSIMD128StackSlot(op->index(),
                                       Translation::FLOAT64x2_STACK_SLOT);
  } else if (op->IsInt32x4StackSlot()) {
    translation->StoreSIMD128StackSlot(op->index(),
                                       Translation::INT32x4_STACK_SLOT);
  } else if (op->IsRegister()) {
    Register reg = ToRegister(op);
    if (is_tagged) {
      translation->StoreRegister(reg);
    } else if (is_uint32) {
      translation->StoreUint32Register(reg);
    } else {
      translation->StoreInt32Register(reg);
    }
  } else if (op->IsDoubleRegister()) {
    XMMRegister reg = ToDoubleRegister(op);
    translation->StoreDoubleRegister(reg);
  } else if (op->IsFloat32x4Register()) {
    XMMRegister reg = ToFloat32x4Register(op);
    translation->StoreSIMD128Register(reg, Translation::FLOAT32x4_REGISTER);
  } else if (op->IsFloat64x2Register()) {
    XMMRegister reg = ToFloat64x2Register(op);
    translation->StoreSIMD128Register(reg, Translation::FLOAT64x2_REGISTER);
  } else if (op->IsInt32x4Register()) {
    XMMRegister reg = ToInt32x4Register(op);
    translation->StoreSIMD128Register(reg, Translation::INT32x4_REGISTER);
  } else if (op->IsConstantOperand()) {
    HConstant* constant = chunk()->LookupConstant(LConstantOperand::cast(op));
    int src_index = DefineDeoptimizationLiteral(constant->handle(isolate()));
    translation->StoreLiteral(src_index);
  } else {
    UNREACHABLE();
  }
}


void LCodeGen::CallCodeGeneric(Handle<Code> code,
                               RelocInfo::Mode mode,
                               LInstruction* instr,
                               SafepointMode safepoint_mode) {
  DCHECK(instr != NULL);
  __ call(code, mode);
  RecordSafepointWithLazyDeopt(instr, safepoint_mode);

  // Signal that we don't inline smi code before these stubs in the
  // optimizing code generator.
  if (code->kind() == Code::BINARY_OP_IC ||
      code->kind() == Code::COMPARE_IC) {
    __ nop();
  }
}


void LCodeGen::CallCode(Handle<Code> code,
                        RelocInfo::Mode mode,
                        LInstruction* instr) {
  CallCodeGeneric(code, mode, instr, RECORD_SIMPLE_SAFEPOINT);
}


void LCodeGen::CallRuntime(const Runtime::Function* fun,
                           int argc,
                           LInstruction* instr,
                           SaveFPRegsMode save_doubles) {
  DCHECK(instr != NULL);
  DCHECK(instr->HasPointerMap());

  __ CallRuntime(fun, argc, save_doubles);

  RecordSafepointWithLazyDeopt(instr, RECORD_SIMPLE_SAFEPOINT);

  DCHECK(info()->is_calling());
}


void LCodeGen::LoadContextFromDeferred(LOperand* context) {
  if (context->IsRegister()) {
    if (!ToRegister(context).is(esi)) {
      __ mov(esi, ToRegister(context));
    }
  } else if (context->IsStackSlot()) {
    __ mov(esi, ToOperand(context));
  } else if (context->IsConstantOperand()) {
    HConstant* constant =
        chunk_->LookupConstant(LConstantOperand::cast(context));
    __ LoadObject(esi, Handle<Object>::cast(constant->handle(isolate())));
  } else {
    UNREACHABLE();
  }
}

void LCodeGen::CallRuntimeFromDeferred(Runtime::FunctionId id,
                                       int argc,
                                       LInstruction* instr,
                                       LOperand* context) {
  LoadContextFromDeferred(context);

  __ CallRuntimeSaveDoubles(id);
  RecordSafepointWithRegisters(
      instr->pointer_map(), argc, Safepoint::kNoLazyDeopt);

  DCHECK(info()->is_calling());
}


void LCodeGen::RegisterEnvironmentForDeoptimization(
    LEnvironment* environment, Safepoint::DeoptMode mode) {
  environment->set_has_been_used();
  if (!environment->HasBeenRegistered()) {
    // Physical stack frame layout:
    // -x ............. -4  0 ..................................... y
    // [incoming arguments] [spill slots] [pushed outgoing arguments]

    // Layout of the environment:
    // 0 ..................................................... size-1
    // [parameters] [locals] [expression stack including arguments]

    // Layout of the translation:
    // 0 ........................................................ size - 1 + 4
    // [expression stack including arguments] [locals] [4 words] [parameters]
    // |>------------  translation_size ------------<|

    int frame_count = 0;
    int jsframe_count = 0;
    for (LEnvironment* e = environment; e != NULL; e = e->outer()) {
      ++frame_count;
      if (e->frame_type() == JS_FUNCTION) {
        ++jsframe_count;
      }
    }
    Translation translation(&translations_, frame_count, jsframe_count, zone());
    WriteTranslation(environment, &translation);
    int deoptimization_index = deoptimizations_.length();
    int pc_offset = masm()->pc_offset();
    environment->Register(deoptimization_index,
                          translation.index(),
                          (mode == Safepoint::kLazyDeopt) ? pc_offset : -1);
    deoptimizations_.Add(environment, zone());
  }
}


void LCodeGen::DeoptimizeIf(Condition cc, LInstruction* instr,
                            Deoptimizer::DeoptReason deopt_reason,
                            Deoptimizer::BailoutType bailout_type) {
  LEnvironment* environment = instr->environment();
  RegisterEnvironmentForDeoptimization(environment, Safepoint::kNoLazyDeopt);
  DCHECK(environment->HasBeenRegistered());
  int id = environment->deoptimization_index();
  DCHECK(info()->IsOptimizing() || info()->IsStub());
  Address entry =
      Deoptimizer::GetDeoptimizationEntry(isolate(), id, bailout_type);
  if (entry == NULL) {
    Abort(kBailoutWasNotPrepared);
    return;
  }

  if (DeoptEveryNTimes()) {
    ExternalReference count = ExternalReference::stress_deopt_count(isolate());
    Label no_deopt;
    __ pushfd();
    __ push(eax);
    __ mov(eax, Operand::StaticVariable(count));
    __ sub(eax, Immediate(1));
    __ j(not_zero, &no_deopt, Label::kNear);
    if (FLAG_trap_on_deopt) __ int3();
    __ mov(eax, Immediate(FLAG_deopt_every_n_times));
    __ mov(Operand::StaticVariable(count), eax);
    __ pop(eax);
    __ popfd();
    DCHECK(frame_is_built_);
    __ call(entry, RelocInfo::RUNTIME_ENTRY);
    __ bind(&no_deopt);
    __ mov(Operand::StaticVariable(count), eax);
    __ pop(eax);
    __ popfd();
  }

  if (info()->ShouldTrapOnDeopt()) {
    Label done;
    if (cc != no_condition) __ j(NegateCondition(cc), &done, Label::kNear);
    __ int3();
    __ bind(&done);
  }

  Deoptimizer::DeoptInfo deopt_info = MakeDeoptInfo(instr, deopt_reason);

  DCHECK(info()->IsStub() || frame_is_built_);
  if (cc == no_condition && frame_is_built_) {
    DeoptComment(deopt_info);
    __ call(entry, RelocInfo::RUNTIME_ENTRY);
    info()->LogDeoptCallPosition(masm()->pc_offset(), deopt_info.inlining_id);
  } else {
    Deoptimizer::JumpTableEntry table_entry(entry, deopt_info, bailout_type,
                                            !frame_is_built_);
    // We often have several deopts to the same entry, reuse the last
    // jump entry if this is the case.
    if (FLAG_trace_deopt || isolate()->cpu_profiler()->is_profiling() ||
        jump_table_.is_empty() ||
        !table_entry.IsEquivalentTo(jump_table_.last())) {
      jump_table_.Add(table_entry, zone());
    }
    if (cc == no_condition) {
      __ jmp(&jump_table_.last().label);
    } else {
      __ j(cc, &jump_table_.last().label);
    }
  }
}


void LCodeGen::DeoptimizeIf(Condition cc, LInstruction* instr,
                            Deoptimizer::DeoptReason deopt_reason) {
  Deoptimizer::BailoutType bailout_type = info()->IsStub()
      ? Deoptimizer::LAZY
      : Deoptimizer::EAGER;
  DeoptimizeIf(cc, instr, deopt_reason, bailout_type);
}


void LCodeGen::PopulateDeoptimizationData(Handle<Code> code) {
  int length = deoptimizations_.length();
  if (length == 0) return;
  Handle<DeoptimizationInputData> data =
      DeoptimizationInputData::New(isolate(), length, TENURED);

  Handle<ByteArray> translations =
      translations_.CreateByteArray(isolate()->factory());
  data->SetTranslationByteArray(*translations);
  data->SetInlinedFunctionCount(Smi::FromInt(inlined_function_count_));
  data->SetOptimizationId(Smi::FromInt(info_->optimization_id()));
  if (info_->IsOptimizing()) {
    // Reference to shared function info does not change between phases.
    AllowDeferredHandleDereference allow_handle_dereference;
    data->SetSharedFunctionInfo(*info_->shared_info());
  } else {
    data->SetSharedFunctionInfo(Smi::FromInt(0));
  }
  data->SetWeakCellCache(Smi::FromInt(0));

  Handle<FixedArray> literals =
      factory()->NewFixedArray(deoptimization_literals_.length(), TENURED);
  { AllowDeferredHandleDereference copy_handles;
    for (int i = 0; i < deoptimization_literals_.length(); i++) {
      literals->set(i, *deoptimization_literals_[i]);
    }
    data->SetLiteralArray(*literals);
  }

  data->SetOsrAstId(Smi::FromInt(info_->osr_ast_id().ToInt()));
  data->SetOsrPcOffset(Smi::FromInt(osr_pc_offset_));

  // Populate the deoptimization entries.
  for (int i = 0; i < length; i++) {
    LEnvironment* env = deoptimizations_[i];
    data->SetAstId(i, env->ast_id());
    data->SetTranslationIndex(i, Smi::FromInt(env->translation_index()));
    data->SetArgumentsStackHeight(i,
                                  Smi::FromInt(env->arguments_stack_height()));
    data->SetPc(i, Smi::FromInt(env->pc_offset()));
  }
  code->set_deoptimization_data(*data);
}


void LCodeGen::PopulateDeoptimizationLiteralsWithInlinedFunctions() {
  DCHECK_EQ(0, deoptimization_literals_.length());
  for (auto function : chunk()->inlined_functions()) {
    DefineDeoptimizationLiteral(function);
  }
  inlined_function_count_ = deoptimization_literals_.length();
}


void LCodeGen::RecordSafepointWithLazyDeopt(
    LInstruction* instr, SafepointMode safepoint_mode) {
  if (safepoint_mode == RECORD_SIMPLE_SAFEPOINT) {
    RecordSafepoint(instr->pointer_map(), Safepoint::kLazyDeopt);
  } else {
    DCHECK(safepoint_mode == RECORD_SAFEPOINT_WITH_REGISTERS_AND_NO_ARGUMENTS);
    RecordSafepointWithRegisters(
        instr->pointer_map(), 0, Safepoint::kLazyDeopt);
  }
}


void LCodeGen::RecordSafepoint(
    LPointerMap* pointers,
    Safepoint::Kind kind,
    int arguments,
    Safepoint::DeoptMode deopt_mode) {
  DCHECK(kind == expected_safepoint_kind_);
  const ZoneList<LOperand*>* operands = pointers->GetNormalizedOperands();
  Safepoint safepoint =
      safepoints_.DefineSafepoint(masm(), kind, arguments, deopt_mode);
  for (int i = 0; i < operands->length(); i++) {
    LOperand* pointer = operands->at(i);
    if (pointer->IsStackSlot()) {
      safepoint.DefinePointerSlot(pointer->index(), zone());
    } else if (pointer->IsRegister() && (kind & Safepoint::kWithRegisters)) {
      safepoint.DefinePointerRegister(ToRegister(pointer), zone());
    }
  }
}


void LCodeGen::RecordSafepoint(LPointerMap* pointers,
                               Safepoint::DeoptMode mode) {
  RecordSafepoint(pointers, Safepoint::kSimple, 0, mode);
}


void LCodeGen::RecordSafepoint(Safepoint::DeoptMode mode) {
  LPointerMap empty_pointers(zone());
  RecordSafepoint(&empty_pointers, mode);
}


void LCodeGen::RecordSafepointWithRegisters(LPointerMap* pointers,
                                            int arguments,
                                            Safepoint::DeoptMode mode) {
  RecordSafepoint(pointers, Safepoint::kWithRegisters, arguments, mode);
}


void LCodeGen::RecordAndWritePosition(int position) {
  if (position == RelocInfo::kNoPosition) return;
  masm()->positions_recorder()->RecordPosition(position);
  masm()->positions_recorder()->WriteRecordedPositions();
}


static const char* LabelType(LLabel* label) {
  if (label->is_loop_header()) return " (loop header)";
  if (label->is_osr_entry()) return " (OSR entry)";
  return "";
}


void LCodeGen::DoLabel(LLabel* label) {
  Comment(";;; <@%d,#%d> -------------------- B%d%s --------------------",
          current_instruction_,
          label->hydrogen_value()->id(),
          label->block_id(),
          LabelType(label));
  __ bind(label->label());
  current_block_ = label->block_id();
  DoGap(label);
}


void LCodeGen::DoParallelMove(LParallelMove* move) {
  resolver_.Resolve(move);
}


void LCodeGen::DoGap(LGap* gap) {
  for (int i = LGap::FIRST_INNER_POSITION;
       i <= LGap::LAST_INNER_POSITION;
       i++) {
    LGap::InnerPosition inner_pos = static_cast<LGap::InnerPosition>(i);
    LParallelMove* move = gap->GetParallelMove(inner_pos);
    if (move != NULL) DoParallelMove(move);
  }
}


void LCodeGen::DoInstructionGap(LInstructionGap* instr) {
  DoGap(instr);
}


void LCodeGen::DoParameter(LParameter* instr) {
  // Nothing to do.
}


void LCodeGen::DoCallStub(LCallStub* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->result()).is(eax));
  switch (instr->hydrogen()->major_key()) {
    case CodeStub::RegExpExec: {
      RegExpExecStub stub(isolate());
      CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
      break;
    }
    case CodeStub::SubString: {
      SubStringStub stub(isolate());
      CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
      break;
    }
    case CodeStub::StringCompare: {
      StringCompareStub stub(isolate());
      CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
      break;
    }
    default:
      UNREACHABLE();
  }
}


void LCodeGen::DoUnknownOSRValue(LUnknownOSRValue* instr) {
  GenerateOsrPrologue();
}


void LCodeGen::DoModByPowerOf2I(LModByPowerOf2I* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  DCHECK(dividend.is(ToRegister(instr->result())));

  // Theoretically, a variation of the branch-free code for integer division by
  // a power of 2 (calculating the remainder via an additional multiplication
  // (which gets simplified to an 'and') and subtraction) should be faster, and
  // this is exactly what GCC and clang emit. Nevertheless, benchmarks seem to
  // indicate that positive dividends are heavily favored, so the branching
  // version performs better.
  HMod* hmod = instr->hydrogen();
  int32_t mask = divisor < 0 ? -(divisor + 1) : (divisor - 1);
  Label dividend_is_not_negative, done;
  if (hmod->CheckFlag(HValue::kLeftCanBeNegative)) {
    __ test(dividend, dividend);
    __ j(not_sign, &dividend_is_not_negative, Label::kNear);
    // Note that this is correct even for kMinInt operands.
    __ neg(dividend);
    __ and_(dividend, mask);
    __ neg(dividend);
    if (hmod->CheckFlag(HValue::kBailoutOnMinusZero)) {
      DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
    }
    __ jmp(&done, Label::kNear);
  }

  __ bind(&dividend_is_not_negative);
  __ and_(dividend, mask);
  __ bind(&done);
}


void LCodeGen::DoModByConstI(LModByConstI* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  DCHECK(ToRegister(instr->result()).is(eax));

  if (divisor == 0) {
    DeoptimizeIf(no_condition, instr, Deoptimizer::kDivisionByZero);
    return;
  }

  __ TruncatingDiv(dividend, Abs(divisor));
  __ imul(edx, edx, Abs(divisor));
  __ mov(eax, dividend);
  __ sub(eax, edx);

  // Check for negative zero.
  HMod* hmod = instr->hydrogen();
  if (hmod->CheckFlag(HValue::kBailoutOnMinusZero)) {
    Label remainder_not_zero;
    __ j(not_zero, &remainder_not_zero, Label::kNear);
    __ cmp(dividend, Immediate(0));
    DeoptimizeIf(less, instr, Deoptimizer::kMinusZero);
    __ bind(&remainder_not_zero);
  }
}


void LCodeGen::DoModI(LModI* instr) {
  HMod* hmod = instr->hydrogen();

  Register left_reg = ToRegister(instr->left());
  DCHECK(left_reg.is(eax));
  Register right_reg = ToRegister(instr->right());
  DCHECK(!right_reg.is(eax));
  DCHECK(!right_reg.is(edx));
  Register result_reg = ToRegister(instr->result());
  DCHECK(result_reg.is(edx));

  Label done;
  // Check for x % 0, idiv would signal a divide error. We have to
  // deopt in this case because we can't return a NaN.
  if (hmod->CheckFlag(HValue::kCanBeDivByZero)) {
    __ test(right_reg, Operand(right_reg));
    DeoptimizeIf(zero, instr, Deoptimizer::kDivisionByZero);
  }

  // Check for kMinInt % -1, idiv would signal a divide error. We
  // have to deopt if we care about -0, because we can't return that.
  if (hmod->CheckFlag(HValue::kCanOverflow)) {
    Label no_overflow_possible;
    __ cmp(left_reg, kMinInt);
    __ j(not_equal, &no_overflow_possible, Label::kNear);
    __ cmp(right_reg, -1);
    if (hmod->CheckFlag(HValue::kBailoutOnMinusZero)) {
      DeoptimizeIf(equal, instr, Deoptimizer::kMinusZero);
    } else {
      __ j(not_equal, &no_overflow_possible, Label::kNear);
      __ Move(result_reg, Immediate(0));
      __ jmp(&done, Label::kNear);
    }
    __ bind(&no_overflow_possible);
  }

  // Sign extend dividend in eax into edx:eax.
  __ cdq();

  // If we care about -0, test if the dividend is <0 and the result is 0.
  if (hmod->CheckFlag(HValue::kBailoutOnMinusZero)) {
    Label positive_left;
    __ test(left_reg, Operand(left_reg));
    __ j(not_sign, &positive_left, Label::kNear);
    __ idiv(right_reg);
    __ test(result_reg, Operand(result_reg));
    DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
    __ jmp(&done, Label::kNear);
    __ bind(&positive_left);
  }
  __ idiv(right_reg);
  __ bind(&done);
}


void LCodeGen::DoDivByPowerOf2I(LDivByPowerOf2I* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  Register result = ToRegister(instr->result());
  DCHECK(divisor == kMinInt || base::bits::IsPowerOfTwo32(Abs(divisor)));
  DCHECK(!result.is(dividend));

  // Check for (0 / -x) that will produce negative zero.
  HDiv* hdiv = instr->hydrogen();
  if (hdiv->CheckFlag(HValue::kBailoutOnMinusZero) && divisor < 0) {
    __ test(dividend, dividend);
    DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
  }
  // Check for (kMinInt / -1).
  if (hdiv->CheckFlag(HValue::kCanOverflow) && divisor == -1) {
    __ cmp(dividend, kMinInt);
    DeoptimizeIf(zero, instr, Deoptimizer::kOverflow);
  }
  // Deoptimize if remainder will not be 0.
  if (!hdiv->CheckFlag(HInstruction::kAllUsesTruncatingToInt32) &&
      divisor != 1 && divisor != -1) {
    int32_t mask = divisor < 0 ? -(divisor + 1) : (divisor - 1);
    __ test(dividend, Immediate(mask));
    DeoptimizeIf(not_zero, instr, Deoptimizer::kLostPrecision);
  }
  __ Move(result, dividend);
  int32_t shift = WhichPowerOf2Abs(divisor);
  if (shift > 0) {
    // The arithmetic shift is always OK, the 'if' is an optimization only.
    if (shift > 1) __ sar(result, 31);
    __ shr(result, 32 - shift);
    __ add(result, dividend);
    __ sar(result, shift);
  }
  if (divisor < 0) __ neg(result);
}


void LCodeGen::DoDivByConstI(LDivByConstI* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  DCHECK(ToRegister(instr->result()).is(edx));

  if (divisor == 0) {
    DeoptimizeIf(no_condition, instr, Deoptimizer::kDivisionByZero);
    return;
  }

  // Check for (0 / -x) that will produce negative zero.
  HDiv* hdiv = instr->hydrogen();
  if (hdiv->CheckFlag(HValue::kBailoutOnMinusZero) && divisor < 0) {
    __ test(dividend, dividend);
    DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
  }

  __ TruncatingDiv(dividend, Abs(divisor));
  if (divisor < 0) __ neg(edx);

  if (!hdiv->CheckFlag(HInstruction::kAllUsesTruncatingToInt32)) {
    __ mov(eax, edx);
    __ imul(eax, eax, divisor);
    __ sub(eax, dividend);
    DeoptimizeIf(not_equal, instr, Deoptimizer::kLostPrecision);
  }
}


// TODO(svenpanne) Refactor this to avoid code duplication with DoFlooringDivI.
void LCodeGen::DoDivI(LDivI* instr) {
  HBinaryOperation* hdiv = instr->hydrogen();
  Register dividend = ToRegister(instr->dividend());
  Register divisor = ToRegister(instr->divisor());
  Register remainder = ToRegister(instr->temp());
  DCHECK(dividend.is(eax));
  DCHECK(remainder.is(edx));
  DCHECK(ToRegister(instr->result()).is(eax));
  DCHECK(!divisor.is(eax));
  DCHECK(!divisor.is(edx));

  // Check for x / 0.
  if (hdiv->CheckFlag(HValue::kCanBeDivByZero)) {
    __ test(divisor, divisor);
    DeoptimizeIf(zero, instr, Deoptimizer::kDivisionByZero);
  }

  // Check for (0 / -x) that will produce negative zero.
  if (hdiv->CheckFlag(HValue::kBailoutOnMinusZero)) {
    Label dividend_not_zero;
    __ test(dividend, dividend);
    __ j(not_zero, &dividend_not_zero, Label::kNear);
    __ test(divisor, divisor);
    DeoptimizeIf(sign, instr, Deoptimizer::kMinusZero);
    __ bind(&dividend_not_zero);
  }

  // Check for (kMinInt / -1).
  if (hdiv->CheckFlag(HValue::kCanOverflow)) {
    Label dividend_not_min_int;
    __ cmp(dividend, kMinInt);
    __ j(not_zero, &dividend_not_min_int, Label::kNear);
    __ cmp(divisor, -1);
    DeoptimizeIf(zero, instr, Deoptimizer::kOverflow);
    __ bind(&dividend_not_min_int);
  }

  // Sign extend to edx (= remainder).
  __ cdq();
  __ idiv(divisor);

  if (!hdiv->CheckFlag(HValue::kAllUsesTruncatingToInt32)) {
    // Deoptimize if remainder is not 0.
    __ test(remainder, remainder);
    DeoptimizeIf(not_zero, instr, Deoptimizer::kLostPrecision);
  }
}


void LCodeGen::DoFlooringDivByPowerOf2I(LFlooringDivByPowerOf2I* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  DCHECK(dividend.is(ToRegister(instr->result())));

  // If the divisor is positive, things are easy: There can be no deopts and we
  // can simply do an arithmetic right shift.
  if (divisor == 1) return;
  int32_t shift = WhichPowerOf2Abs(divisor);
  if (divisor > 1) {
    __ sar(dividend, shift);
    return;
  }

  // If the divisor is negative, we have to negate and handle edge cases.
  __ neg(dividend);
  if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
    DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
  }

  // Dividing by -1 is basically negation, unless we overflow.
  if (divisor == -1) {
    if (instr->hydrogen()->CheckFlag(HValue::kLeftCanBeMinInt)) {
      DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
    }
    return;
  }

  // If the negation could not overflow, simply shifting is OK.
  if (!instr->hydrogen()->CheckFlag(HValue::kLeftCanBeMinInt)) {
    __ sar(dividend, shift);
    return;
  }

  Label not_kmin_int, done;
  __ j(no_overflow, &not_kmin_int, Label::kNear);
  __ mov(dividend, Immediate(kMinInt / divisor));
  __ jmp(&done, Label::kNear);
  __ bind(&not_kmin_int);
  __ sar(dividend, shift);
  __ bind(&done);
}


void LCodeGen::DoFlooringDivByConstI(LFlooringDivByConstI* instr) {
  Register dividend = ToRegister(instr->dividend());
  int32_t divisor = instr->divisor();
  DCHECK(ToRegister(instr->result()).is(edx));

  if (divisor == 0) {
    DeoptimizeIf(no_condition, instr, Deoptimizer::kDivisionByZero);
    return;
  }

  // Check for (0 / -x) that will produce negative zero.
  HMathFloorOfDiv* hdiv = instr->hydrogen();
  if (hdiv->CheckFlag(HValue::kBailoutOnMinusZero) && divisor < 0) {
    __ test(dividend, dividend);
    DeoptimizeIf(zero, instr, Deoptimizer::kMinusZero);
  }

  // Easy case: We need no dynamic check for the dividend and the flooring
  // division is the same as the truncating division.
  if ((divisor > 0 && !hdiv->CheckFlag(HValue::kLeftCanBeNegative)) ||
      (divisor < 0 && !hdiv->CheckFlag(HValue::kLeftCanBePositive))) {
    __ TruncatingDiv(dividend, Abs(divisor));
    if (divisor < 0) __ neg(edx);
    return;
  }

  // In the general case we may need to adjust before and after the truncating
  // division to get a flooring division.
  Register temp = ToRegister(instr->temp3());
  DCHECK(!temp.is(dividend) && !temp.is(eax) && !temp.is(edx));
  Label needs_adjustment, done;
  __ cmp(dividend, Immediate(0));
  __ j(divisor > 0 ? less : greater, &needs_adjustment, Label::kNear);
  __ TruncatingDiv(dividend, Abs(divisor));
  if (divisor < 0) __ neg(edx);
  __ jmp(&done, Label::kNear);
  __ bind(&needs_adjustment);
  __ lea(temp, Operand(dividend, divisor > 0 ? 1 : -1));
  __ TruncatingDiv(temp, Abs(divisor));
  if (divisor < 0) __ neg(edx);
  __ dec(edx);
  __ bind(&done);
}


// TODO(svenpanne) Refactor this to avoid code duplication with DoDivI.
void LCodeGen::DoFlooringDivI(LFlooringDivI* instr) {
  HBinaryOperation* hdiv = instr->hydrogen();
  Register dividend = ToRegister(instr->dividend());
  Register divisor = ToRegister(instr->divisor());
  Register remainder = ToRegister(instr->temp());
  Register result = ToRegister(instr->result());
  DCHECK(dividend.is(eax));
  DCHECK(remainder.is(edx));
  DCHECK(result.is(eax));
  DCHECK(!divisor.is(eax));
  DCHECK(!divisor.is(edx));

  // Check for x / 0.
  if (hdiv->CheckFlag(HValue::kCanBeDivByZero)) {
    __ test(divisor, divisor);
    DeoptimizeIf(zero, instr, Deoptimizer::kDivisionByZero);
  }

  // Check for (0 / -x) that will produce negative zero.
  if (hdiv->CheckFlag(HValue::kBailoutOnMinusZero)) {
    Label dividend_not_zero;
    __ test(dividend, dividend);
    __ j(not_zero, &dividend_not_zero, Label::kNear);
    __ test(divisor, divisor);
    DeoptimizeIf(sign, instr, Deoptimizer::kMinusZero);
    __ bind(&dividend_not_zero);
  }

  // Check for (kMinInt / -1).
  if (hdiv->CheckFlag(HValue::kCanOverflow)) {
    Label dividend_not_min_int;
    __ cmp(dividend, kMinInt);
    __ j(not_zero, &dividend_not_min_int, Label::kNear);
    __ cmp(divisor, -1);
    DeoptimizeIf(zero, instr, Deoptimizer::kOverflow);
    __ bind(&dividend_not_min_int);
  }

  // Sign extend to edx (= remainder).
  __ cdq();
  __ idiv(divisor);

  Label done;
  __ test(remainder, remainder);
  __ j(zero, &done, Label::kNear);
  __ xor_(remainder, divisor);
  __ sar(remainder, 31);
  __ add(result, remainder);
  __ bind(&done);
}


void LCodeGen::DoMulI(LMulI* instr) {
  Register left = ToRegister(instr->left());
  LOperand* right = instr->right();

  if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
    __ mov(ToRegister(instr->temp()), left);
  }

  if (right->IsConstantOperand()) {
    // Try strength reductions on the multiplication.
    // All replacement instructions are at most as long as the imul
    // and have better latency.
    int constant = ToInteger32(LConstantOperand::cast(right));
    if (constant == -1) {
      __ neg(left);
    } else if (constant == 0) {
      __ xor_(left, Operand(left));
    } else if (constant == 2) {
      __ add(left, Operand(left));
    } else if (!instr->hydrogen()->CheckFlag(HValue::kCanOverflow)) {
      // If we know that the multiplication can't overflow, it's safe to
      // use instructions that don't set the overflow flag for the
      // multiplication.
      switch (constant) {
        case 1:
          // Do nothing.
          break;
        case 3:
          __ lea(left, Operand(left, left, times_2, 0));
          break;
        case 4:
          __ shl(left, 2);
          break;
        case 5:
          __ lea(left, Operand(left, left, times_4, 0));
          break;
        case 8:
          __ shl(left, 3);
          break;
        case 9:
          __ lea(left, Operand(left, left, times_8, 0));
          break;
        case 16:
          __ shl(left, 4);
          break;
        default:
          __ imul(left, left, constant);
          break;
      }
    } else {
      __ imul(left, left, constant);
    }
  } else {
    if (instr->hydrogen()->representation().IsSmi()) {
      __ SmiUntag(left);
    }
    __ imul(left, ToOperand(right));
  }

  if (instr->hydrogen()->CheckFlag(HValue::kCanOverflow)) {
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
  }

  if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
    // Bail out if the result is supposed to be negative zero.
    Label done;
    __ test(left, Operand(left));
    __ j(not_zero, &done, Label::kNear);
    if (right->IsConstantOperand()) {
      if (ToInteger32(LConstantOperand::cast(right)) < 0) {
        DeoptimizeIf(no_condition, instr, Deoptimizer::kMinusZero);
      } else if (ToInteger32(LConstantOperand::cast(right)) == 0) {
        __ cmp(ToRegister(instr->temp()), Immediate(0));
        DeoptimizeIf(less, instr, Deoptimizer::kMinusZero);
      }
    } else {
      // Test the non-zero operand for negative sign.
      __ or_(ToRegister(instr->temp()), ToOperand(right));
      DeoptimizeIf(sign, instr, Deoptimizer::kMinusZero);
    }
    __ bind(&done);
  }
}


void LCodeGen::DoBitI(LBitI* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();
  DCHECK(left->Equals(instr->result()));
  DCHECK(left->IsRegister());

  if (right->IsConstantOperand()) {
    int32_t right_operand =
        ToRepresentation(LConstantOperand::cast(right),
                         instr->hydrogen()->representation());
    switch (instr->op()) {
      case Token::BIT_AND:
        __ and_(ToRegister(left), right_operand);
        break;
      case Token::BIT_OR:
        __ or_(ToRegister(left), right_operand);
        break;
      case Token::BIT_XOR:
        if (right_operand == int32_t(~0)) {
          __ not_(ToRegister(left));
        } else {
          __ xor_(ToRegister(left), right_operand);
        }
        break;
      default:
        UNREACHABLE();
        break;
    }
  } else {
    switch (instr->op()) {
      case Token::BIT_AND:
        __ and_(ToRegister(left), ToOperand(right));
        break;
      case Token::BIT_OR:
        __ or_(ToRegister(left), ToOperand(right));
        break;
      case Token::BIT_XOR:
        __ xor_(ToRegister(left), ToOperand(right));
        break;
      default:
        UNREACHABLE();
        break;
    }
  }
}


void LCodeGen::DoShiftI(LShiftI* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();
  DCHECK(left->Equals(instr->result()));
  DCHECK(left->IsRegister());
  if (right->IsRegister()) {
    DCHECK(ToRegister(right).is(ecx));

    switch (instr->op()) {
      case Token::ROR:
        __ ror_cl(ToRegister(left));
        break;
      case Token::SAR:
        __ sar_cl(ToRegister(left));
        break;
      case Token::SHR:
        __ shr_cl(ToRegister(left));
        if (instr->can_deopt()) {
          __ test(ToRegister(left), ToRegister(left));
          DeoptimizeIf(sign, instr, Deoptimizer::kNegativeValue);
        }
        break;
      case Token::SHL:
        __ shl_cl(ToRegister(left));
        break;
      default:
        UNREACHABLE();
        break;
    }
  } else {
    int value = ToInteger32(LConstantOperand::cast(right));
    uint8_t shift_count = static_cast<uint8_t>(value & 0x1F);
    switch (instr->op()) {
      case Token::ROR:
        if (shift_count == 0 && instr->can_deopt()) {
          __ test(ToRegister(left), ToRegister(left));
          DeoptimizeIf(sign, instr, Deoptimizer::kNegativeValue);
        } else {
          __ ror(ToRegister(left), shift_count);
        }
        break;
      case Token::SAR:
        if (shift_count != 0) {
          __ sar(ToRegister(left), shift_count);
        }
        break;
      case Token::SHR:
        if (shift_count != 0) {
          __ shr(ToRegister(left), shift_count);
        } else if (instr->can_deopt()) {
          __ test(ToRegister(left), ToRegister(left));
          DeoptimizeIf(sign, instr, Deoptimizer::kNegativeValue);
        }
        break;
      case Token::SHL:
        if (shift_count != 0) {
          if (instr->hydrogen_value()->representation().IsSmi() &&
              instr->can_deopt()) {
            if (shift_count != 1) {
              __ shl(ToRegister(left), shift_count - 1);
            }
            __ SmiTag(ToRegister(left));
            DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
          } else {
            __ shl(ToRegister(left), shift_count);
          }
        }
        break;
      default:
        UNREACHABLE();
        break;
    }
  }
}


void LCodeGen::DoSubI(LSubI* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();
  DCHECK(left->Equals(instr->result()));

  if (right->IsConstantOperand()) {
    __ sub(ToOperand(left),
           ToImmediate(right, instr->hydrogen()->representation()));
  } else {
    __ sub(ToRegister(left), ToOperand(right));
  }
  if (instr->hydrogen()->CheckFlag(HValue::kCanOverflow)) {
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
  }
}


void LCodeGen::DoConstantI(LConstantI* instr) {
  __ Move(ToRegister(instr->result()), Immediate(instr->value()));
}


void LCodeGen::DoConstantS(LConstantS* instr) {
  __ Move(ToRegister(instr->result()), Immediate(instr->value()));
}


void LCodeGen::DoConstantD(LConstantD* instr) {
  uint64_t const bits = instr->bits();
  uint32_t const lower = static_cast<uint32_t>(bits);
  uint32_t const upper = static_cast<uint32_t>(bits >> 32);
  DCHECK(instr->result()->IsDoubleRegister());

  XMMRegister result = ToDoubleRegister(instr->result());
  if (bits == 0u) {
    __ xorps(result, result);
  } else {
    Register temp = ToRegister(instr->temp());
    if (CpuFeatures::IsSupported(SSE4_1)) {
      CpuFeatureScope scope2(masm(), SSE4_1);
      if (lower != 0) {
        __ Move(temp, Immediate(lower));
        __ movd(result, Operand(temp));
        __ Move(temp, Immediate(upper));
        __ pinsrd(result, Operand(temp), 1);
      } else {
        __ xorps(result, result);
        __ Move(temp, Immediate(upper));
        __ pinsrd(result, Operand(temp), 1);
      }
    } else {
      __ Move(temp, Immediate(upper));
      __ movd(result, Operand(temp));
      __ psllq(result, 32);
      if (lower != 0u) {
        XMMRegister xmm_scratch = double_scratch0();
        __ Move(temp, Immediate(lower));
        __ movd(xmm_scratch, Operand(temp));
        __ orps(result, xmm_scratch);
      }
    }
  }
}


void LCodeGen::DoConstantE(LConstantE* instr) {
  __ lea(ToRegister(instr->result()), Operand::StaticVariable(instr->value()));
}


void LCodeGen::DoConstantT(LConstantT* instr) {
  Register reg = ToRegister(instr->result());
  Handle<Object> object = instr->value(isolate());
  AllowDeferredHandleDereference smi_check;
  __ LoadObject(reg, object);
}


void LCodeGen::DoMapEnumLength(LMapEnumLength* instr) {
  Register result = ToRegister(instr->result());
  Register map = ToRegister(instr->value());
  __ EnumLength(result, map);
}


void LCodeGen::DoDateField(LDateField* instr) {
  Register object = ToRegister(instr->date());
  Register result = ToRegister(instr->result());
  Register scratch = ToRegister(instr->temp());
  Smi* index = instr->index();
  DCHECK(object.is(result));
  DCHECK(object.is(eax));

  if (index->value() == 0) {
    __ mov(result, FieldOperand(object, JSDate::kValueOffset));
  } else {
    Label runtime, done;
    if (index->value() < JSDate::kFirstUncachedField) {
      ExternalReference stamp = ExternalReference::date_cache_stamp(isolate());
      __ mov(scratch, Operand::StaticVariable(stamp));
      __ cmp(scratch, FieldOperand(object, JSDate::kCacheStampOffset));
      __ j(not_equal, &runtime, Label::kNear);
      __ mov(result, FieldOperand(object, JSDate::kValueOffset +
                                          kPointerSize * index->value()));
      __ jmp(&done, Label::kNear);
    }
    __ bind(&runtime);
    __ PrepareCallCFunction(2, scratch);
    __ mov(Operand(esp, 0), object);
    __ mov(Operand(esp, 1 * kPointerSize), Immediate(index));
    __ CallCFunction(ExternalReference::get_date_field_function(isolate()), 2);
    __ bind(&done);
  }
}


Operand LCodeGen::BuildSeqStringOperand(Register string,
                                        LOperand* index,
                                        String::Encoding encoding) {
  if (index->IsConstantOperand()) {
    int offset = ToRepresentation(LConstantOperand::cast(index),
                                  Representation::Integer32());
    if (encoding == String::TWO_BYTE_ENCODING) {
      offset *= kUC16Size;
    }
    STATIC_ASSERT(kCharSize == 1);
    return FieldOperand(string, SeqString::kHeaderSize + offset);
  }
  return FieldOperand(
      string, ToRegister(index),
      encoding == String::ONE_BYTE_ENCODING ? times_1 : times_2,
      SeqString::kHeaderSize);
}


void LCodeGen::DoSeqStringGetChar(LSeqStringGetChar* instr) {
  String::Encoding encoding = instr->hydrogen()->encoding();
  Register result = ToRegister(instr->result());
  Register string = ToRegister(instr->string());

  if (FLAG_debug_code) {
    __ push(string);
    __ mov(string, FieldOperand(string, HeapObject::kMapOffset));
    __ movzx_b(string, FieldOperand(string, Map::kInstanceTypeOffset));

    __ and_(string, Immediate(kStringRepresentationMask | kStringEncodingMask));
    static const uint32_t one_byte_seq_type = kSeqStringTag | kOneByteStringTag;
    static const uint32_t two_byte_seq_type = kSeqStringTag | kTwoByteStringTag;
    __ cmp(string, Immediate(encoding == String::ONE_BYTE_ENCODING
                             ? one_byte_seq_type : two_byte_seq_type));
    __ Check(equal, kUnexpectedStringType);
    __ pop(string);
  }

  Operand operand = BuildSeqStringOperand(string, instr->index(), encoding);
  if (encoding == String::ONE_BYTE_ENCODING) {
    __ movzx_b(result, operand);
  } else {
    __ movzx_w(result, operand);
  }
}


void LCodeGen::DoSeqStringSetChar(LSeqStringSetChar* instr) {
  String::Encoding encoding = instr->hydrogen()->encoding();
  Register string = ToRegister(instr->string());

  if (FLAG_debug_code) {
    Register value = ToRegister(instr->value());
    Register index = ToRegister(instr->index());
    static const uint32_t one_byte_seq_type = kSeqStringTag | kOneByteStringTag;
    static const uint32_t two_byte_seq_type = kSeqStringTag | kTwoByteStringTag;
    int encoding_mask =
        instr->hydrogen()->encoding() == String::ONE_BYTE_ENCODING
        ? one_byte_seq_type : two_byte_seq_type;
    __ EmitSeqStringSetCharCheck(string, index, value, encoding_mask);
  }

  Operand operand = BuildSeqStringOperand(string, instr->index(), encoding);
  if (instr->value()->IsConstantOperand()) {
    int value = ToRepresentation(LConstantOperand::cast(instr->value()),
                                 Representation::Integer32());
    DCHECK_LE(0, value);
    if (encoding == String::ONE_BYTE_ENCODING) {
      DCHECK_LE(value, String::kMaxOneByteCharCode);
      __ mov_b(operand, static_cast<int8_t>(value));
    } else {
      DCHECK_LE(value, String::kMaxUtf16CodeUnit);
      __ mov_w(operand, static_cast<int16_t>(value));
    }
  } else {
    Register value = ToRegister(instr->value());
    if (encoding == String::ONE_BYTE_ENCODING) {
      __ mov_b(operand, value);
    } else {
      __ mov_w(operand, value);
    }
  }
}


void LCodeGen::DoAddI(LAddI* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();

  if (LAddI::UseLea(instr->hydrogen()) && !left->Equals(instr->result())) {
    if (right->IsConstantOperand()) {
      int32_t offset = ToRepresentation(LConstantOperand::cast(right),
                                        instr->hydrogen()->representation());
      __ lea(ToRegister(instr->result()), MemOperand(ToRegister(left), offset));
    } else {
      Operand address(ToRegister(left), ToRegister(right), times_1, 0);
      __ lea(ToRegister(instr->result()), address);
    }
  } else {
    if (right->IsConstantOperand()) {
      __ add(ToOperand(left),
             ToImmediate(right, instr->hydrogen()->representation()));
    } else {
      __ add(ToRegister(left), ToOperand(right));
    }
    if (instr->hydrogen()->CheckFlag(HValue::kCanOverflow)) {
      DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
    }
  }
}


void LCodeGen::DoMathMinMax(LMathMinMax* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();
  DCHECK(left->Equals(instr->result()));
  HMathMinMax::Operation operation = instr->hydrogen()->operation();
  if (instr->hydrogen()->representation().IsSmiOrInteger32()) {
    Label return_left;
    Condition condition = (operation == HMathMinMax::kMathMin)
        ? less_equal
        : greater_equal;
    if (right->IsConstantOperand()) {
      Operand left_op = ToOperand(left);
      Immediate immediate = ToImmediate(LConstantOperand::cast(instr->right()),
                                        instr->hydrogen()->representation());
      __ cmp(left_op, immediate);
      __ j(condition, &return_left, Label::kNear);
      __ mov(left_op, immediate);
    } else {
      Register left_reg = ToRegister(left);
      Operand right_op = ToOperand(right);
      __ cmp(left_reg, right_op);
      __ j(condition, &return_left, Label::kNear);
      __ mov(left_reg, right_op);
    }
    __ bind(&return_left);
  } else {
    DCHECK(instr->hydrogen()->representation().IsDouble());
    Label check_nan_left, check_zero, return_left, return_right;
    Condition condition = (operation == HMathMinMax::kMathMin) ? below : above;
    XMMRegister left_reg = ToDoubleRegister(left);
    XMMRegister right_reg = ToDoubleRegister(right);
    __ ucomisd(left_reg, right_reg);
    __ j(parity_even, &check_nan_left, Label::kNear);  // At least one NaN.
    __ j(equal, &check_zero, Label::kNear);  // left == right.
    __ j(condition, &return_left, Label::kNear);
    __ jmp(&return_right, Label::kNear);

    __ bind(&check_zero);
    XMMRegister xmm_scratch = double_scratch0();
    __ xorps(xmm_scratch, xmm_scratch);
    __ ucomisd(left_reg, xmm_scratch);
    __ j(not_equal, &return_left, Label::kNear);  // left == right != 0.
    // At this point, both left and right are either 0 or -0.
    if (operation == HMathMinMax::kMathMin) {
      __ orpd(left_reg, right_reg);
    } else {
      // Since we operate on +0 and/or -0, addsd and andsd have the same effect.
      __ addsd(left_reg, right_reg);
    }
    __ jmp(&return_left, Label::kNear);

    __ bind(&check_nan_left);
    __ ucomisd(left_reg, left_reg);  // NaN check.
    __ j(parity_even, &return_left, Label::kNear);  // left == NaN.
    __ bind(&return_right);
    __ movaps(left_reg, right_reg);

    __ bind(&return_left);
  }
}


void LCodeGen::DoArithmeticD(LArithmeticD* instr) {
  XMMRegister left = ToDoubleRegister(instr->left());
  XMMRegister right = ToDoubleRegister(instr->right());
  XMMRegister result = ToDoubleRegister(instr->result());
  switch (instr->op()) {
    case Token::ADD:
      if (CpuFeatures::IsSupported(AVX)) {
        CpuFeatureScope scope(masm(), AVX);
        __ vaddsd(result, left, right);
      } else {
        DCHECK(result.is(left));
        __ addsd(left, right);
      }
      break;
    case Token::SUB:
      if (CpuFeatures::IsSupported(AVX)) {
        CpuFeatureScope scope(masm(), AVX);
        __ vsubsd(result, left, right);
      } else {
        DCHECK(result.is(left));
        __ subsd(left, right);
      }
      break;
    case Token::MUL:
      if (CpuFeatures::IsSupported(AVX)) {
        CpuFeatureScope scope(masm(), AVX);
        __ vmulsd(result, left, right);
      } else {
        DCHECK(result.is(left));
        __ mulsd(left, right);
      }
      break;
    case Token::DIV:
      if (CpuFeatures::IsSupported(AVX)) {
        CpuFeatureScope scope(masm(), AVX);
        __ vdivsd(result, left, right);
      } else {
        DCHECK(result.is(left));
        __ divsd(left, right);
      }
      // Don't delete this mov. It may improve performance on some CPUs,
      // when there is a (v)mulsd depending on the result
      __ movaps(result, result);
      break;
    case Token::MOD: {
      // Pass two doubles as arguments on the stack.
      __ PrepareCallCFunction(4, eax);
      __ movsd(Operand(esp, 0 * kDoubleSize), left);
      __ movsd(Operand(esp, 1 * kDoubleSize), right);
      __ CallCFunction(
          ExternalReference::mod_two_doubles_operation(isolate()),
          4);

      // Return value is in st(0) on ia32.
      // Store it into the result register.
      __ sub(Operand(esp), Immediate(kDoubleSize));
      __ fstp_d(Operand(esp, 0));
      __ movsd(result, Operand(esp, 0));
      __ add(Operand(esp), Immediate(kDoubleSize));
      break;
    }
    default:
      UNREACHABLE();
      break;
  }
}


void LCodeGen::DoArithmeticT(LArithmeticT* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->left()).is(edx));
  DCHECK(ToRegister(instr->right()).is(eax));
  DCHECK(ToRegister(instr->result()).is(eax));

  Handle<Code> code =
      CodeFactory::BinaryOpIC(isolate(), instr->op(), instr->strength()).code();
  CallCode(code, RelocInfo::CODE_TARGET, instr);
}


template<class InstrType>
void LCodeGen::EmitBranch(InstrType instr, Condition cc) {
  int left_block = instr->TrueDestination(chunk_);
  int right_block = instr->FalseDestination(chunk_);

  int next_block = GetNextEmittedBlock();

  if (right_block == left_block || cc == no_condition) {
    EmitGoto(left_block);
  } else if (left_block == next_block) {
    __ j(NegateCondition(cc), chunk_->GetAssemblyLabel(right_block));
  } else if (right_block == next_block) {
    __ j(cc, chunk_->GetAssemblyLabel(left_block));
  } else {
    __ j(cc, chunk_->GetAssemblyLabel(left_block));
    __ jmp(chunk_->GetAssemblyLabel(right_block));
  }
}


template<class InstrType>
void LCodeGen::EmitFalseBranch(InstrType instr, Condition cc) {
  int false_block = instr->FalseDestination(chunk_);
  if (cc == no_condition) {
    __ jmp(chunk_->GetAssemblyLabel(false_block));
  } else {
    __ j(cc, chunk_->GetAssemblyLabel(false_block));
  }
}


void LCodeGen::DoBranch(LBranch* instr) {
  Representation r = instr->hydrogen()->value()->representation();
  if (r.IsSmiOrInteger32()) {
    Register reg = ToRegister(instr->value());
    __ test(reg, Operand(reg));
    EmitBranch(instr, not_zero);
  } else if (r.IsDouble()) {
    DCHECK(!info()->IsStub());
    XMMRegister reg = ToDoubleRegister(instr->value());
    XMMRegister xmm_scratch = double_scratch0();
    __ xorps(xmm_scratch, xmm_scratch);
    __ ucomisd(reg, xmm_scratch);
    EmitBranch(instr, not_equal);
  } else if (r.IsSIMD128()) {
    DCHECK(!info()->IsStub());
    EmitBranch(instr, no_condition);
  } else {
    DCHECK(r.IsTagged());
    Register reg = ToRegister(instr->value());
    HType type = instr->hydrogen()->value()->type();
    if (type.IsBoolean()) {
      DCHECK(!info()->IsStub());
      __ cmp(reg, factory()->true_value());
      EmitBranch(instr, equal);
    } else if (type.IsSmi()) {
      DCHECK(!info()->IsStub());
      __ test(reg, Operand(reg));
      EmitBranch(instr, not_equal);
    } else if (type.IsJSArray()) {
      DCHECK(!info()->IsStub());
      EmitBranch(instr, no_condition);
    } else if (type.IsHeapNumber()) {
      DCHECK(!info()->IsStub());
      XMMRegister xmm_scratch = double_scratch0();
      __ xorps(xmm_scratch, xmm_scratch);
      __ ucomisd(xmm_scratch, FieldOperand(reg, HeapNumber::kValueOffset));
      EmitBranch(instr, not_equal);
    } else if (type.IsString()) {
      DCHECK(!info()->IsStub());
      __ cmp(FieldOperand(reg, String::kLengthOffset), Immediate(0));
      EmitBranch(instr, not_equal);
    } else {
      ToBooleanStub::Types expected = instr->hydrogen()->expected_input_types();
      if (expected.IsEmpty()) expected = ToBooleanStub::Types::Generic();

      if (expected.Contains(ToBooleanStub::UNDEFINED)) {
        // undefined -> false.
        __ cmp(reg, factory()->undefined_value());
        __ j(equal, instr->FalseLabel(chunk_));
      }
      if (expected.Contains(ToBooleanStub::BOOLEAN)) {
        // true -> true.
        __ cmp(reg, factory()->true_value());
        __ j(equal, instr->TrueLabel(chunk_));
        // false -> false.
        __ cmp(reg, factory()->false_value());
        __ j(equal, instr->FalseLabel(chunk_));
      }
      if (expected.Contains(ToBooleanStub::NULL_TYPE)) {
        // 'null' -> false.
        __ cmp(reg, factory()->null_value());
        __ j(equal, instr->FalseLabel(chunk_));
      }

      if (expected.Contains(ToBooleanStub::SMI)) {
        // Smis: 0 -> false, all other -> true.
        __ test(reg, Operand(reg));
        __ j(equal, instr->FalseLabel(chunk_));
        __ JumpIfSmi(reg, instr->TrueLabel(chunk_));
      } else if (expected.NeedsMap()) {
        // If we need a map later and have a Smi -> deopt.
        __ test(reg, Immediate(kSmiTagMask));
        DeoptimizeIf(zero, instr, Deoptimizer::kSmi);
      }

      Register map = no_reg;  // Keep the compiler happy.
      if (expected.NeedsMap()) {
        map = ToRegister(instr->temp());
        DCHECK(!map.is(reg));
        __ mov(map, FieldOperand(reg, HeapObject::kMapOffset));

        if (expected.CanBeUndetectable()) {
          // Undetectable -> false.
          __ test_b(FieldOperand(map, Map::kBitFieldOffset),
                    1 << Map::kIsUndetectable);
          __ j(not_zero, instr->FalseLabel(chunk_));
        }
      }

      if (expected.Contains(ToBooleanStub::SPEC_OBJECT)) {
        // spec object -> true.
        __ CmpInstanceType(map, FIRST_SPEC_OBJECT_TYPE);
        __ j(above_equal, instr->TrueLabel(chunk_));
      }

      if (expected.Contains(ToBooleanStub::STRING)) {
        // String value -> false iff empty.
        Label not_string;
        __ CmpInstanceType(map, FIRST_NONSTRING_TYPE);
        __ j(above_equal, &not_string, Label::kNear);
        __ cmp(FieldOperand(reg, String::kLengthOffset), Immediate(0));
        __ j(not_zero, instr->TrueLabel(chunk_));
        __ jmp(instr->FalseLabel(chunk_));
        __ bind(&not_string);
      }

      if (expected.Contains(ToBooleanStub::SYMBOL)) {
        // Symbol value -> true.
        __ CmpInstanceType(map, SYMBOL_TYPE);
        __ j(equal, instr->TrueLabel(chunk_));
      }

      if (expected.Contains(ToBooleanStub::HEAP_NUMBER)) {
        // heap number -> false iff +0, -0, or NaN.
        Label not_heap_number;
        __ cmp(FieldOperand(reg, HeapObject::kMapOffset),
               factory()->heap_number_map());
        __ j(not_equal, &not_heap_number, Label::kNear);
        XMMRegister xmm_scratch = double_scratch0();
        __ xorps(xmm_scratch, xmm_scratch);
        __ ucomisd(xmm_scratch, FieldOperand(reg, HeapNumber::kValueOffset));
        __ j(zero, instr->FalseLabel(chunk_));
        __ jmp(instr->TrueLabel(chunk_));
        __ bind(&not_heap_number);
      }

      if (!expected.IsGeneric()) {
        // We've seen something for the first time -> deopt.
        // This can only happen if we are not generic already.
        DeoptimizeIf(no_condition, instr, Deoptimizer::kUnexpectedObject);
      }
    }
  }
}


void LCodeGen::EmitGoto(int block) {
  if (!IsNextEmittedBlock(block)) {
    __ jmp(chunk_->GetAssemblyLabel(LookupDestination(block)));
  }
}


void LCodeGen::DoGoto(LGoto* instr) {
  EmitGoto(instr->block_id());
}


Condition LCodeGen::TokenToCondition(Token::Value op, bool is_unsigned) {
  Condition cond = no_condition;
  switch (op) {
    case Token::EQ:
    case Token::EQ_STRICT:
      cond = equal;
      break;
    case Token::NE:
    case Token::NE_STRICT:
      cond = not_equal;
      break;
    case Token::LT:
      cond = is_unsigned ? below : less;
      break;
    case Token::GT:
      cond = is_unsigned ? above : greater;
      break;
    case Token::LTE:
      cond = is_unsigned ? below_equal : less_equal;
      break;
    case Token::GTE:
      cond = is_unsigned ? above_equal : greater_equal;
      break;
    case Token::IN:
    case Token::INSTANCEOF:
    default:
      UNREACHABLE();
  }
  return cond;
}


void LCodeGen::DoCompareNumericAndBranch(LCompareNumericAndBranch* instr) {
  LOperand* left = instr->left();
  LOperand* right = instr->right();
  bool is_unsigned =
      instr->is_double() ||
      instr->hydrogen()->left()->CheckFlag(HInstruction::kUint32) ||
      instr->hydrogen()->right()->CheckFlag(HInstruction::kUint32);
  Condition cc = TokenToCondition(instr->op(), is_unsigned);

  if (left->IsConstantOperand() && right->IsConstantOperand()) {
    // We can statically evaluate the comparison.
    double left_val = ToDouble(LConstantOperand::cast(left));
    double right_val = ToDouble(LConstantOperand::cast(right));
    int next_block = EvalComparison(instr->op(), left_val, right_val) ?
        instr->TrueDestination(chunk_) : instr->FalseDestination(chunk_);
    EmitGoto(next_block);
  } else {
    if (instr->is_double()) {
      __ ucomisd(ToDoubleRegister(left), ToDoubleRegister(right));
      // Don't base result on EFLAGS when a NaN is involved. Instead
      // jump to the false block.
      __ j(parity_even, instr->FalseLabel(chunk_));
    } else {
      if (right->IsConstantOperand()) {
        __ cmp(ToOperand(left),
               ToImmediate(right, instr->hydrogen()->representation()));
      } else if (left->IsConstantOperand()) {
        __ cmp(ToOperand(right),
               ToImmediate(left, instr->hydrogen()->representation()));
        // We commuted the operands, so commute the condition.
        cc = CommuteCondition(cc);
      } else {
        __ cmp(ToRegister(left), ToOperand(right));
      }
    }
    EmitBranch(instr, cc);
  }
}


void LCodeGen::DoCmpObjectEqAndBranch(LCmpObjectEqAndBranch* instr) {
  Register left = ToRegister(instr->left());

  if (instr->right()->IsConstantOperand()) {
    Handle<Object> right = ToHandle(LConstantOperand::cast(instr->right()));
    __ CmpObject(left, right);
  } else {
    Operand right = ToOperand(instr->right());
    __ cmp(left, right);
  }
  EmitBranch(instr, equal);
}


void LCodeGen::DoCmpHoleAndBranch(LCmpHoleAndBranch* instr) {
  if (instr->hydrogen()->representation().IsTagged()) {
    Register input_reg = ToRegister(instr->object());
    __ cmp(input_reg, factory()->the_hole_value());
    EmitBranch(instr, equal);
    return;
  }

  XMMRegister input_reg = ToDoubleRegister(instr->object());
  __ ucomisd(input_reg, input_reg);
  EmitFalseBranch(instr, parity_odd);

  __ sub(esp, Immediate(kDoubleSize));
  __ movsd(MemOperand(esp, 0), input_reg);

  __ add(esp, Immediate(kDoubleSize));
  int offset = sizeof(kHoleNanUpper32);
  __ cmp(MemOperand(esp, -offset), Immediate(kHoleNanUpper32));
  EmitBranch(instr, equal);
}


void LCodeGen::DoCompareMinusZeroAndBranch(LCompareMinusZeroAndBranch* instr) {
  Representation rep = instr->hydrogen()->value()->representation();
  DCHECK(!rep.IsInteger32());
  Register scratch = ToRegister(instr->temp());

  if (rep.IsDouble()) {
    XMMRegister value = ToDoubleRegister(instr->value());
    XMMRegister xmm_scratch = double_scratch0();
    __ xorps(xmm_scratch, xmm_scratch);
    __ ucomisd(xmm_scratch, value);
    EmitFalseBranch(instr, not_equal);
    __ movmskpd(scratch, value);
    __ test(scratch, Immediate(1));
    EmitBranch(instr, not_zero);
  } else {
    Register value = ToRegister(instr->value());
    Handle<Map> map = masm()->isolate()->factory()->heap_number_map();
    __ CheckMap(value, map, instr->FalseLabel(chunk()), DO_SMI_CHECK);
    __ cmp(FieldOperand(value, HeapNumber::kExponentOffset),
           Immediate(0x1));
    EmitFalseBranch(instr, no_overflow);
    __ cmp(FieldOperand(value, HeapNumber::kMantissaOffset),
           Immediate(0x00000000));
    EmitBranch(instr, equal);
  }
}


Condition LCodeGen::EmitIsObject(Register input,
                                 Register temp1,
                                 Label* is_not_object,
                                 Label* is_object) {
  __ JumpIfSmi(input, is_not_object);

  __ cmp(input, isolate()->factory()->null_value());
  __ j(equal, is_object);

  __ mov(temp1, FieldOperand(input, HeapObject::kMapOffset));
  // Undetectable objects behave like undefined.
  __ test_b(FieldOperand(temp1, Map::kBitFieldOffset),
            1 << Map::kIsUndetectable);
  __ j(not_zero, is_not_object);

  __ movzx_b(temp1, FieldOperand(temp1, Map::kInstanceTypeOffset));
  __ cmp(temp1, FIRST_NONCALLABLE_SPEC_OBJECT_TYPE);
  __ j(below, is_not_object);
  __ cmp(temp1, LAST_NONCALLABLE_SPEC_OBJECT_TYPE);
  return below_equal;
}


void LCodeGen::DoIsObjectAndBranch(LIsObjectAndBranch* instr) {
  Register reg = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  Condition true_cond = EmitIsObject(
      reg, temp, instr->FalseLabel(chunk_), instr->TrueLabel(chunk_));

  EmitBranch(instr, true_cond);
}


Condition LCodeGen::EmitIsString(Register input,
                                 Register temp1,
                                 Label* is_not_string,
                                 SmiCheck check_needed = INLINE_SMI_CHECK) {
  if (check_needed == INLINE_SMI_CHECK) {
    __ JumpIfSmi(input, is_not_string);
  }

  Condition cond = masm_->IsObjectStringType(input, temp1, temp1);

  return cond;
}


void LCodeGen::DoIsStringAndBranch(LIsStringAndBranch* instr) {
  Register reg = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  SmiCheck check_needed =
      instr->hydrogen()->value()->type().IsHeapObject()
          ? OMIT_SMI_CHECK : INLINE_SMI_CHECK;

  Condition true_cond = EmitIsString(
      reg, temp, instr->FalseLabel(chunk_), check_needed);

  EmitBranch(instr, true_cond);
}


void LCodeGen::DoIsSmiAndBranch(LIsSmiAndBranch* instr) {
  Operand input = ToOperand(instr->value());

  __ test(input, Immediate(kSmiTagMask));
  EmitBranch(instr, zero);
}


void LCodeGen::DoIsUndetectableAndBranch(LIsUndetectableAndBranch* instr) {
  Register input = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  if (!instr->hydrogen()->value()->type().IsHeapObject()) {
    STATIC_ASSERT(kSmiTag == 0);
    __ JumpIfSmi(input, instr->FalseLabel(chunk_));
  }
  __ mov(temp, FieldOperand(input, HeapObject::kMapOffset));
  __ test_b(FieldOperand(temp, Map::kBitFieldOffset),
            1 << Map::kIsUndetectable);
  EmitBranch(instr, not_zero);
}


static Condition ComputeCompareCondition(Token::Value op) {
  switch (op) {
    case Token::EQ_STRICT:
    case Token::EQ:
      return equal;
    case Token::LT:
      return less;
    case Token::GT:
      return greater;
    case Token::LTE:
      return less_equal;
    case Token::GTE:
      return greater_equal;
    default:
      UNREACHABLE();
      return no_condition;
  }
}


void LCodeGen::DoStringCompareAndBranch(LStringCompareAndBranch* instr) {
  Token::Value op = instr->op();

  Handle<Code> ic =
      CodeFactory::CompareIC(isolate(), op, Strength::WEAK).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);

  Condition condition = ComputeCompareCondition(op);
  __ test(eax, Operand(eax));

  EmitBranch(instr, condition);
}


static InstanceType TestType(HHasInstanceTypeAndBranch* instr) {
  InstanceType from = instr->from();
  InstanceType to = instr->to();
  if (from == FIRST_TYPE) return to;
  DCHECK(from == to || to == LAST_TYPE);
  return from;
}


static Condition BranchCondition(HHasInstanceTypeAndBranch* instr) {
  InstanceType from = instr->from();
  InstanceType to = instr->to();
  if (from == to) return equal;
  if (to == LAST_TYPE) return above_equal;
  if (from == FIRST_TYPE) return below_equal;
  UNREACHABLE();
  return equal;
}


void LCodeGen::DoHasInstanceTypeAndBranch(LHasInstanceTypeAndBranch* instr) {
  Register input = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  if (!instr->hydrogen()->value()->type().IsHeapObject()) {
    __ JumpIfSmi(input, instr->FalseLabel(chunk_));
  }

  __ CmpObjectType(input, TestType(instr->hydrogen()), temp);
  EmitBranch(instr, BranchCondition(instr->hydrogen()));
}


void LCodeGen::DoGetCachedArrayIndex(LGetCachedArrayIndex* instr) {
  Register input = ToRegister(instr->value());
  Register result = ToRegister(instr->result());

  __ AssertString(input);

  __ mov(result, FieldOperand(input, String::kHashFieldOffset));
  __ IndexFromHash(result, result);
}


void LCodeGen::DoHasCachedArrayIndexAndBranch(
    LHasCachedArrayIndexAndBranch* instr) {
  Register input = ToRegister(instr->value());

  __ test(FieldOperand(input, String::kHashFieldOffset),
          Immediate(String::kContainsCachedArrayIndexMask));
  EmitBranch(instr, equal);
}


// Branches to a label or falls through with the answer in the z flag.  Trashes
// the temp registers, but not the input.
void LCodeGen::EmitClassOfTest(Label* is_true,
                               Label* is_false,
                               Handle<String>class_name,
                               Register input,
                               Register temp,
                               Register temp2) {
  DCHECK(!input.is(temp));
  DCHECK(!input.is(temp2));
  DCHECK(!temp.is(temp2));
  __ JumpIfSmi(input, is_false);

  if (String::Equals(isolate()->factory()->Function_string(), class_name)) {
    // Assuming the following assertions, we can use the same compares to test
    // for both being a function type and being in the object type range.
    STATIC_ASSERT(NUM_OF_CALLABLE_SPEC_OBJECT_TYPES == 2);
    STATIC_ASSERT(FIRST_NONCALLABLE_SPEC_OBJECT_TYPE ==
                  FIRST_SPEC_OBJECT_TYPE + 1);
    STATIC_ASSERT(LAST_NONCALLABLE_SPEC_OBJECT_TYPE ==
                  LAST_SPEC_OBJECT_TYPE - 1);
    STATIC_ASSERT(LAST_SPEC_OBJECT_TYPE == LAST_TYPE);
    __ CmpObjectType(input, FIRST_SPEC_OBJECT_TYPE, temp);
    __ j(below, is_false);
    __ j(equal, is_true);
    __ CmpInstanceType(temp, LAST_SPEC_OBJECT_TYPE);
    __ j(equal, is_true);
  } else {
    // Faster code path to avoid two compares: subtract lower bound from the
    // actual type and do a signed compare with the width of the type range.
    __ mov(temp, FieldOperand(input, HeapObject::kMapOffset));
    __ movzx_b(temp2, FieldOperand(temp, Map::kInstanceTypeOffset));
    __ sub(Operand(temp2), Immediate(FIRST_NONCALLABLE_SPEC_OBJECT_TYPE));
    __ cmp(Operand(temp2), Immediate(LAST_NONCALLABLE_SPEC_OBJECT_TYPE -
                                     FIRST_NONCALLABLE_SPEC_OBJECT_TYPE));
    __ j(above, is_false);
  }

  // Now we are in the FIRST-LAST_NONCALLABLE_SPEC_OBJECT_TYPE range.
  // Check if the constructor in the map is a function.
  __ GetMapConstructor(temp, temp, temp2);
  // Objects with a non-function constructor have class 'Object'.
  __ CmpInstanceType(temp2, JS_FUNCTION_TYPE);
  if (String::Equals(class_name, isolate()->factory()->Object_string())) {
    __ j(not_equal, is_true);
  } else {
    __ j(not_equal, is_false);
  }

  // temp now contains the constructor function. Grab the
  // instance class name from there.
  __ mov(temp, FieldOperand(temp, JSFunction::kSharedFunctionInfoOffset));
  __ mov(temp, FieldOperand(temp,
                            SharedFunctionInfo::kInstanceClassNameOffset));
  // The class name we are testing against is internalized since it's a literal.
  // The name in the constructor is internalized because of the way the context
  // is booted.  This routine isn't expected to work for random API-created
  // classes and it doesn't have to because you can't access it with natives
  // syntax.  Since both sides are internalized it is sufficient to use an
  // identity comparison.
  __ cmp(temp, class_name);
  // End with the answer in the z flag.
}


void LCodeGen::DoClassOfTestAndBranch(LClassOfTestAndBranch* instr) {
  Register input = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());
  Register temp2 = ToRegister(instr->temp2());

  Handle<String> class_name = instr->hydrogen()->class_name();

  EmitClassOfTest(instr->TrueLabel(chunk_), instr->FalseLabel(chunk_),
      class_name, input, temp, temp2);

  EmitBranch(instr, equal);
}


void LCodeGen::DoCmpMapAndBranch(LCmpMapAndBranch* instr) {
  Register reg = ToRegister(instr->value());
  __ cmp(FieldOperand(reg, HeapObject::kMapOffset), instr->map());
  EmitBranch(instr, equal);
}


void LCodeGen::DoInstanceOf(LInstanceOf* instr) {
  // Object and function are in fixed registers defined by the stub.
  DCHECK(ToRegister(instr->context()).is(esi));
  InstanceofStub stub(isolate(), InstanceofStub::kArgsInRegisters);
  CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);

  Label true_value, done;
  __ test(eax, Operand(eax));
  __ j(zero, &true_value, Label::kNear);
  __ mov(ToRegister(instr->result()), factory()->false_value());
  __ jmp(&done, Label::kNear);
  __ bind(&true_value);
  __ mov(ToRegister(instr->result()), factory()->true_value());
  __ bind(&done);
}


void LCodeGen::DoInstanceOfKnownGlobal(LInstanceOfKnownGlobal* instr) {
  class DeferredInstanceOfKnownGlobal final : public LDeferredCode {
   public:
    DeferredInstanceOfKnownGlobal(LCodeGen* codegen,
                                  LInstanceOfKnownGlobal* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override {
      codegen()->DoDeferredInstanceOfKnownGlobal(instr_, &map_check_);
    }
    LInstruction* instr() override { return instr_; }
    Label* map_check() { return &map_check_; }
   private:
    LInstanceOfKnownGlobal* instr_;
    Label map_check_;
  };

  DeferredInstanceOfKnownGlobal* deferred;
  deferred = new(zone()) DeferredInstanceOfKnownGlobal(this, instr);

  Label done, false_result;
  Register object = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  // A Smi is not an instance of anything.
  __ JumpIfSmi(object, &false_result, Label::kNear);

  // This is the inlined call site instanceof cache. The two occurences of the
  // hole value will be patched to the last map/result pair generated by the
  // instanceof stub.
  Label cache_miss;
  Register map = ToRegister(instr->temp());
  __ mov(map, FieldOperand(object, HeapObject::kMapOffset));
  __ bind(deferred->map_check());  // Label for calculating code patching.
  Handle<Cell> cache_cell = factory()->NewCell(factory()->the_hole_value());
  __ cmp(map, Operand::ForCell(cache_cell));  // Patched to cached map.
  __ j(not_equal, &cache_miss, Label::kNear);
  __ mov(eax, factory()->the_hole_value());  // Patched to either true or false.
  __ jmp(&done, Label::kNear);

  // The inlined call site cache did not match. Check for null and string
  // before calling the deferred code.
  __ bind(&cache_miss);
  // Null is not an instance of anything.
  __ cmp(object, factory()->null_value());
  __ j(equal, &false_result, Label::kNear);

  // String values are not instances of anything.
  Condition is_string = masm_->IsObjectStringType(object, temp, temp);
  __ j(is_string, &false_result, Label::kNear);

  // Go to the deferred code.
  __ jmp(deferred->entry());

  __ bind(&false_result);
  __ mov(ToRegister(instr->result()), factory()->false_value());

  // Here result has either true or false. Deferred code also produces true or
  // false object.
  __ bind(deferred->exit());
  __ bind(&done);
}


void LCodeGen::DoDeferredInstanceOfKnownGlobal(LInstanceOfKnownGlobal* instr,
                                               Label* map_check) {
  PushSafepointRegistersScope scope(this);

  InstanceofStub::Flags flags = InstanceofStub::kNoFlags;
  flags = static_cast<InstanceofStub::Flags>(
      flags | InstanceofStub::kArgsInRegisters);
  flags = static_cast<InstanceofStub::Flags>(
      flags | InstanceofStub::kCallSiteInlineCheck);
  flags = static_cast<InstanceofStub::Flags>(
      flags | InstanceofStub::kReturnTrueFalseObject);
  InstanceofStub stub(isolate(), flags);

  // Get the temp register reserved by the instruction. This needs to be a
  // register which is pushed last by PushSafepointRegisters as top of the
  // stack is used to pass the offset to the location of the map check to
  // the stub.
  Register temp = ToRegister(instr->temp());
  DCHECK(MacroAssembler::SafepointRegisterStackIndex(temp) == 0);
  __ LoadHeapObject(InstanceofStub::right(), instr->function());
  static const int kAdditionalDelta = 13;
  int delta = masm_->SizeOfCodeGeneratedSince(map_check) + kAdditionalDelta;
  __ mov(temp, Immediate(delta));
  __ StoreToSafepointRegisterSlot(temp, temp);
  CallCodeGeneric(stub.GetCode(),
                  RelocInfo::CODE_TARGET,
                  instr,
                  RECORD_SAFEPOINT_WITH_REGISTERS_AND_NO_ARGUMENTS);
  // Get the deoptimization index of the LLazyBailout-environment that
  // corresponds to this instruction.
  LEnvironment* env = instr->GetDeferredLazyDeoptimizationEnvironment();
  safepoints_.RecordLazyDeoptimizationIndex(env->deoptimization_index());

  // Put the result value into the eax slot and restore all registers.
  __ StoreToSafepointRegisterSlot(eax, eax);
}


void LCodeGen::DoCmpT(LCmpT* instr) {
  Token::Value op = instr->op();

  Handle<Code> ic =
      CodeFactory::CompareIC(isolate(), op, instr->strength()).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);

  Condition condition = ComputeCompareCondition(op);
  Label true_value, done;
  __ test(eax, Operand(eax));
  __ j(condition, &true_value, Label::kNear);
  __ mov(ToRegister(instr->result()), factory()->false_value());
  __ jmp(&done, Label::kNear);
  __ bind(&true_value);
  __ mov(ToRegister(instr->result()), factory()->true_value());
  __ bind(&done);
}


void LCodeGen::EmitReturn(LReturn* instr, bool dynamic_frame_alignment) {
  int extra_value_count = dynamic_frame_alignment ? 2 : 1;

  if (instr->has_constant_parameter_count()) {
    int parameter_count = ToInteger32(instr->constant_parameter_count());
    if (dynamic_frame_alignment && FLAG_debug_code) {
      __ cmp(Operand(esp,
                     (parameter_count + extra_value_count) * kPointerSize),
             Immediate(kAlignmentZapValue));
      __ Assert(equal, kExpectedAlignmentMarker);
    }
    __ Ret((parameter_count + extra_value_count) * kPointerSize, ecx);
  } else {
    DCHECK(info()->IsStub());  // Functions would need to drop one more value.
    Register reg = ToRegister(instr->parameter_count());
    // The argument count parameter is a smi
    __ SmiUntag(reg);
    Register return_addr_reg = reg.is(ecx) ? ebx : ecx;
    if (dynamic_frame_alignment && FLAG_debug_code) {
      DCHECK(extra_value_count == 2);
      __ cmp(Operand(esp, reg, times_pointer_size,
                     extra_value_count * kPointerSize),
             Immediate(kAlignmentZapValue));
      __ Assert(equal, kExpectedAlignmentMarker);
    }

    // emit code to restore stack based on instr->parameter_count()
    __ pop(return_addr_reg);  // save return address
    if (dynamic_frame_alignment) {
      __ inc(reg);  // 1 more for alignment
    }

    __ shl(reg, kPointerSizeLog2);
    __ add(esp, reg);
    __ jmp(return_addr_reg);
  }
}


void LCodeGen::DoReturn(LReturn* instr) {
  if (FLAG_trace && info()->IsOptimizing()) {
    // Preserve the return value on the stack and rely on the runtime call
    // to return the value in the same register.  We're leaving the code
    // managed by the register allocator and tearing down the frame, it's
    // safe to write to the context register.
    __ push(eax);
    __ mov(esi, Operand(ebp, StandardFrameConstants::kContextOffset));
    __ CallRuntime(Runtime::kTraceExit, 1);
  }
  if (info()->saves_caller_doubles()) RestoreCallerDoubles();
  if (dynamic_frame_alignment_) {
    // Fetch the state of the dynamic frame alignment.
    __ mov(edx, Operand(ebp,
      JavaScriptFrameConstants::kDynamicAlignmentStateOffset));
  }
  int no_frame_start = -1;
  if (NeedsEagerFrame()) {
    __ mov(esp, ebp);
    __ pop(ebp);
    no_frame_start = masm_->pc_offset();
  }
  if (dynamic_frame_alignment_) {
    Label no_padding;
    __ cmp(edx, Immediate(kNoAlignmentPadding));
    __ j(equal, &no_padding, Label::kNear);

    EmitReturn(instr, true);
    __ bind(&no_padding);
  }

  EmitReturn(instr, false);
  if (no_frame_start != -1) {
    info()->AddNoFrameRange(no_frame_start, masm_->pc_offset());
  }
}


template <class T>
void LCodeGen::EmitVectorLoadICRegisters(T* instr) {
  Register vector_register = ToRegister(instr->temp_vector());
  Register slot_register = LoadWithVectorDescriptor::SlotRegister();
  DCHECK(vector_register.is(LoadWithVectorDescriptor::VectorRegister()));
  DCHECK(slot_register.is(eax));

  AllowDeferredHandleDereference vector_structure_check;
  Handle<TypeFeedbackVector> vector = instr->hydrogen()->feedback_vector();
  __ mov(vector_register, vector);
  // No need to allocate this register.
  FeedbackVectorICSlot slot = instr->hydrogen()->slot();
  int index = vector->GetIndex(slot);
  __ mov(slot_register, Immediate(Smi::FromInt(index)));
}


template <class T>
void LCodeGen::EmitVectorStoreICRegisters(T* instr) {
  Register vector_register = ToRegister(instr->temp_vector());
  Register slot_register = ToRegister(instr->temp_slot());

  AllowDeferredHandleDereference vector_structure_check;
  Handle<TypeFeedbackVector> vector = instr->hydrogen()->feedback_vector();
  __ mov(vector_register, vector);
  FeedbackVectorICSlot slot = instr->hydrogen()->slot();
  int index = vector->GetIndex(slot);
  __ mov(slot_register, Immediate(Smi::FromInt(index)));
}


void LCodeGen::DoLoadGlobalGeneric(LLoadGlobalGeneric* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->global_object())
             .is(LoadDescriptor::ReceiverRegister()));
  DCHECK(ToRegister(instr->result()).is(eax));

  __ mov(LoadDescriptor::NameRegister(), instr->name());
  EmitVectorLoadICRegisters<LLoadGlobalGeneric>(instr);
  ContextualMode mode = instr->for_typeof() ? NOT_CONTEXTUAL : CONTEXTUAL;
  Handle<Code> ic = CodeFactory::LoadICInOptimizedCode(isolate(), mode, SLOPPY,
                                                       PREMONOMORPHIC).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoLoadContextSlot(LLoadContextSlot* instr) {
  Register context = ToRegister(instr->context());
  Register result = ToRegister(instr->result());
  __ mov(result, ContextOperand(context, instr->slot_index()));

  if (instr->hydrogen()->RequiresHoleCheck()) {
    __ cmp(result, factory()->the_hole_value());
    if (instr->hydrogen()->DeoptimizesOnHole()) {
      DeoptimizeIf(equal, instr, Deoptimizer::kHole);
    } else {
      Label is_not_hole;
      __ j(not_equal, &is_not_hole, Label::kNear);
      __ mov(result, factory()->undefined_value());
      __ bind(&is_not_hole);
    }
  }
}


void LCodeGen::DoStoreContextSlot(LStoreContextSlot* instr) {
  Register context = ToRegister(instr->context());
  Register value = ToRegister(instr->value());

  Label skip_assignment;

  Operand target = ContextOperand(context, instr->slot_index());
  if (instr->hydrogen()->RequiresHoleCheck()) {
    __ cmp(target, factory()->the_hole_value());
    if (instr->hydrogen()->DeoptimizesOnHole()) {
      DeoptimizeIf(equal, instr, Deoptimizer::kHole);
    } else {
      __ j(not_equal, &skip_assignment, Label::kNear);
    }
  }

  __ mov(target, value);
  if (instr->hydrogen()->NeedsWriteBarrier()) {
    SmiCheck check_needed =
        instr->hydrogen()->value()->type().IsHeapObject()
            ? OMIT_SMI_CHECK : INLINE_SMI_CHECK;
    Register temp = ToRegister(instr->temp());
    int offset = Context::SlotOffset(instr->slot_index());
    __ RecordWriteContextSlot(context,
                              offset,
                              value,
                              temp,
                              kSaveFPRegs,
                              EMIT_REMEMBERED_SET,
                              check_needed);
  }

  __ bind(&skip_assignment);
}


void LCodeGen::DoLoadNamedField(LLoadNamedField* instr) {
  HObjectAccess access = instr->hydrogen()->access();
  int offset = access.offset();

  if (access.IsExternalMemory()) {
    Register result = ToRegister(instr->result());
    MemOperand operand = instr->object()->IsConstantOperand()
        ? MemOperand::StaticVariable(ToExternalReference(
                LConstantOperand::cast(instr->object())))
        : MemOperand(ToRegister(instr->object()), offset);
    __ Load(result, operand, access.representation());
    return;
  }

  Register object = ToRegister(instr->object());
  if (instr->hydrogen()->representation().IsDouble()) {
    XMMRegister result = ToDoubleRegister(instr->result());
    __ movsd(result, FieldOperand(object, offset));
    return;
  }

  Register result = ToRegister(instr->result());
  if (!access.IsInobject()) {
    __ mov(result, FieldOperand(object, JSObject::kPropertiesOffset));
    object = result;
  }
  __ Load(result, FieldOperand(object, offset), access.representation());
}


void LCodeGen::EmitPushTaggedOperand(LOperand* operand) {
  DCHECK(!operand->IsDoubleRegister());
  if (operand->IsConstantOperand()) {
    Handle<Object> object = ToHandle(LConstantOperand::cast(operand));
    AllowDeferredHandleDereference smi_check;
    if (object->IsSmi()) {
      __ Push(Handle<Smi>::cast(object));
    } else {
      __ PushHeapObject(Handle<HeapObject>::cast(object));
    }
  } else if (operand->IsRegister()) {
    __ push(ToRegister(operand));
  } else {
    __ push(ToOperand(operand));
  }
}


void LCodeGen::DoLoadNamedGeneric(LLoadNamedGeneric* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->object()).is(LoadDescriptor::ReceiverRegister()));
  DCHECK(ToRegister(instr->result()).is(eax));

  __ mov(LoadDescriptor::NameRegister(), instr->name());
  EmitVectorLoadICRegisters<LLoadNamedGeneric>(instr);
  Handle<Code> ic =
      CodeFactory::LoadICInOptimizedCode(
          isolate(), NOT_CONTEXTUAL, instr->hydrogen()->language_mode(),
          instr->hydrogen()->initialization_state()).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoLoadFunctionPrototype(LLoadFunctionPrototype* instr) {
  Register function = ToRegister(instr->function());
  Register temp = ToRegister(instr->temp());
  Register result = ToRegister(instr->result());

  // Get the prototype or initial map from the function.
  __ mov(result,
         FieldOperand(function, JSFunction::kPrototypeOrInitialMapOffset));

  // Check that the function has a prototype or an initial map.
  __ cmp(Operand(result), Immediate(factory()->the_hole_value()));
  DeoptimizeIf(equal, instr, Deoptimizer::kHole);

  // If the function does not have an initial map, we're done.
  Label done;
  __ CmpObjectType(result, MAP_TYPE, temp);
  __ j(not_equal, &done, Label::kNear);

  // Get the prototype from the initial map.
  __ mov(result, FieldOperand(result, Map::kPrototypeOffset));

  // All done.
  __ bind(&done);
}


void LCodeGen::DoLoadRoot(LLoadRoot* instr) {
  Register result = ToRegister(instr->result());
  __ LoadRoot(result, instr->index());
}


void LCodeGen::DoAccessArgumentsAt(LAccessArgumentsAt* instr) {
  Register arguments = ToRegister(instr->arguments());
  Register result = ToRegister(instr->result());
  if (instr->length()->IsConstantOperand() &&
      instr->index()->IsConstantOperand()) {
    int const_index = ToInteger32(LConstantOperand::cast(instr->index()));
    int const_length = ToInteger32(LConstantOperand::cast(instr->length()));
    int index = (const_length - const_index) + 1;
    __ mov(result, Operand(arguments, index * kPointerSize));
  } else {
    Register length = ToRegister(instr->length());
    Operand index = ToOperand(instr->index());
    // There are two words between the frame pointer and the last argument.
    // Subtracting from length accounts for one of them add one more.
    __ sub(length, index);
    __ mov(result, Operand(arguments, length, times_4, kPointerSize));
  }
}


void LCodeGen::DoDeferredSIMD128ToTagged(LInstruction* instr,
                                         Runtime::FunctionId id) {
  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  Register reg = ToRegister(instr->result());
  __ Move(reg, Immediate(0));

  PushSafepointRegistersScope scope(this);
  __ mov(esi, Operand(ebp, StandardFrameConstants::kContextOffset));
  __ CallRuntimeSaveDoubles(id);
  RecordSafepointWithRegisters(
      instr->pointer_map(), 0, Safepoint::kNoLazyDeopt);
  __ StoreToSafepointRegisterSlot(reg, eax);
}


void LCodeGen::HandleExternalArrayOpRequiresTemp(
    LOperand* key,
    Representation key_representation,
    ElementsKind elements_kind) {
  if (ExternalArrayOpRequiresPreScale(key_representation, elements_kind)) {
    int pre_shift_size = ElementsKindToShiftSize(elements_kind) -
        static_cast<int>(maximal_scale_factor);
    if (key_representation.IsSmi()) {
      pre_shift_size -= kSmiTagSize;
    }
    DCHECK(pre_shift_size > 0);
    __ shl(ToRegister(key), pre_shift_size);
  } else {
    __ SmiUntag(ToRegister(key));
  }
}


void LCodeGen::DoLoadKeyedExternalArray(LLoadKeyed* instr) {
  ElementsKind elements_kind = instr->elements_kind();
  LOperand* key = instr->key();
  if (!key->IsConstantOperand() &&
      ExternalArrayOpRequiresTemp(
          instr->hydrogen()->key()->representation(), elements_kind)) {
    HandleExternalArrayOpRequiresTemp(
        key, instr->hydrogen()->key()->representation(), elements_kind);
  }

  Operand operand(BuildFastArrayOperand(
      instr->elements(),
      key,
      instr->hydrogen()->key()->representation(),
      elements_kind,
      instr->base_offset()));
  BuiltinFunctionId op = instr->hydrogen()->op();
  if (IsSIMD128LoadStoreOp(op)) {
    if (GetSIMD128LoadStoreBytes(op) == 16) {
      __ movups(ToSIMD128Register(instr->result()), operand);
    } else if (GetSIMD128LoadStoreBytes(op) == 4) {
      __ movss(ToSIMD128Register(instr->result()), operand);
    } else if (GetSIMD128LoadStoreBytes(op) == 8) {
      __ movq(ToSIMD128Register(instr->result()), operand);
    } else if (GetSIMD128LoadStoreBytes(op) == 12) {
      XMMRegister result(ToSIMD128Register(instr->result()));
      XMMRegister xmm_scratch = double_scratch0();
      __ movq(result, operand);
      Operand operand2(BuildFastArrayOperand(
          instr->elements(),
          key,
          instr->hydrogen()->key()->representation(),
          elements_kind,
          instr->base_offset() + 8));
      __ movss(xmm_scratch, operand2);
      __ movlhps(result, xmm_scratch);
    }
  } else if (elements_kind == EXTERNAL_FLOAT32_ELEMENTS ||
      elements_kind == FLOAT32_ELEMENTS) {
    XMMRegister result(ToDoubleRegister(instr->result()));
    __ movss(result, operand);
    __ cvtss2sd(result, result);
  } else if (elements_kind == EXTERNAL_FLOAT64_ELEMENTS ||
             elements_kind == FLOAT64_ELEMENTS) {
    __ movsd(ToDoubleRegister(instr->result()), operand);
  } else if (IsSIMD128ElementsKind(elements_kind)) {
    __ movups(ToSIMD128Register(instr->result()), operand);
  } else {
    Register result(ToRegister(instr->result()));
    switch (elements_kind) {
      case EXTERNAL_INT8_ELEMENTS:
      case INT8_ELEMENTS:
        __ movsx_b(result, operand);
        break;
      case EXTERNAL_UINT8_CLAMPED_ELEMENTS:
      case EXTERNAL_UINT8_ELEMENTS:
      case UINT8_ELEMENTS:
      case UINT8_CLAMPED_ELEMENTS:
        __ movzx_b(result, operand);
        break;
      case EXTERNAL_INT16_ELEMENTS:
      case INT16_ELEMENTS:
        __ movsx_w(result, operand);
        break;
      case EXTERNAL_UINT16_ELEMENTS:
      case UINT16_ELEMENTS:
        __ movzx_w(result, operand);
        break;
      case EXTERNAL_INT32_ELEMENTS:
      case INT32_ELEMENTS:
        __ mov(result, operand);
        break;
      case EXTERNAL_UINT32_ELEMENTS:
      case UINT32_ELEMENTS:
        __ mov(result, operand);
        if (!instr->hydrogen()->CheckFlag(HInstruction::kUint32)) {
          __ test(result, Operand(result));
          DeoptimizeIf(negative, instr, Deoptimizer::kNegativeValue);
        }
        break;
      case EXTERNAL_FLOAT32_ELEMENTS:
      case EXTERNAL_FLOAT64_ELEMENTS:
      case FLOAT32_ELEMENTS:
      case FLOAT64_ELEMENTS:
      case FAST_SMI_ELEMENTS:
      case FAST_ELEMENTS:
      case FAST_DOUBLE_ELEMENTS:
      case FAST_HOLEY_SMI_ELEMENTS:
      case FAST_HOLEY_ELEMENTS:
      case FAST_HOLEY_DOUBLE_ELEMENTS:
      case DICTIONARY_ELEMENTS:
      case FAST_SLOPPY_ARGUMENTS_ELEMENTS:
      case SLOW_SLOPPY_ARGUMENTS_ELEMENTS:
        UNREACHABLE();
        break;
    }
  }
}


void LCodeGen::DoLoadKeyedFixedDoubleArray(LLoadKeyed* instr) {
  if (instr->hydrogen()->RequiresHoleCheck()) {
    Operand hole_check_operand = BuildFastArrayOperand(
        instr->elements(), instr->key(),
        instr->hydrogen()->key()->representation(),
        FAST_DOUBLE_ELEMENTS,
        instr->base_offset() + sizeof(kHoleNanLower32));
    __ cmp(hole_check_operand, Immediate(kHoleNanUpper32));
    DeoptimizeIf(equal, instr, Deoptimizer::kHole);
  }

  Operand double_load_operand = BuildFastArrayOperand(
      instr->elements(),
      instr->key(),
      instr->hydrogen()->key()->representation(),
      FAST_DOUBLE_ELEMENTS,
      instr->base_offset());
  XMMRegister result = ToDoubleRegister(instr->result());
  __ movsd(result, double_load_operand);
}


void LCodeGen::DoLoadKeyedFixedArray(LLoadKeyed* instr) {
  Register result = ToRegister(instr->result());

  // Load the result.
  __ mov(result,
         BuildFastArrayOperand(instr->elements(), instr->key(),
                               instr->hydrogen()->key()->representation(),
                               FAST_ELEMENTS, instr->base_offset()));

  // Check for the hole value.
  if (instr->hydrogen()->RequiresHoleCheck()) {
    if (IsFastSmiElementsKind(instr->hydrogen()->elements_kind())) {
      __ test(result, Immediate(kSmiTagMask));
      DeoptimizeIf(not_equal, instr, Deoptimizer::kNotASmi);
    } else {
      __ cmp(result, factory()->the_hole_value());
      DeoptimizeIf(equal, instr, Deoptimizer::kHole);
    }
  } else if (instr->hydrogen()->hole_mode() == CONVERT_HOLE_TO_UNDEFINED) {
    DCHECK(instr->hydrogen()->elements_kind() == FAST_HOLEY_ELEMENTS);
    Label done;
    __ cmp(result, factory()->the_hole_value());
    __ j(not_equal, &done);
    if (info()->IsStub()) {
      // A stub can safely convert the hole to undefined only if the array
      // protector cell contains (Smi) Isolate::kArrayProtectorValid. Otherwise
      // it needs to bail out.
      __ mov(result, isolate()->factory()->array_protector());
      __ cmp(FieldOperand(result, PropertyCell::kValueOffset),
             Immediate(Smi::FromInt(Isolate::kArrayProtectorValid)));
      DeoptimizeIf(not_equal, instr, Deoptimizer::kHole);
    }
    __ mov(result, isolate()->factory()->undefined_value());
    __ bind(&done);
  }
}


void LCodeGen::DoLoadKeyed(LLoadKeyed* instr) {
  if (instr->is_typed_elements()) {
    DoLoadKeyedExternalArray(instr);
  } else if (instr->hydrogen()->representation().IsDouble()) {
    DoLoadKeyedFixedDoubleArray(instr);
  } else {
    DoLoadKeyedFixedArray(instr);
  }
}


Operand LCodeGen::BuildFastArrayOperand(
    LOperand* elements_pointer,
    LOperand* key,
    Representation key_representation,
    ElementsKind elements_kind,
    uint32_t base_offset) {
  Register elements_pointer_reg = ToRegister(elements_pointer);
  int element_shift_size = ElementsKindToShiftSize(elements_kind);
  int shift_size = element_shift_size;
  if (key->IsConstantOperand()) {
    int constant_value = ToInteger32(LConstantOperand::cast(key));
    if (constant_value & 0xF0000000) {
      Abort(kArrayIndexConstantValueTooBig);
    }
    return Operand(elements_pointer_reg,
                   ((constant_value) << shift_size)
                       + base_offset);
  } else {
    if (ExternalArrayOpRequiresPreScale(key_representation, elements_kind)) {
      // Make sure the key is pre-scaled against maximal_scale_factor.
      shift_size = static_cast<int>(maximal_scale_factor);
    } else if (key_representation.IsSmi() && (shift_size >= 1)) {
      // Take the tag bit into account while computing the shift size.
      shift_size -= kSmiTagSize;
    }
    ScaleFactor scale_factor = static_cast<ScaleFactor>(shift_size);
    return Operand(elements_pointer_reg,
                   ToRegister(key),
                   scale_factor,
                   base_offset);
  }
}


void LCodeGen::DoLoadKeyedGeneric(LLoadKeyedGeneric* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->object()).is(LoadDescriptor::ReceiverRegister()));
  DCHECK(ToRegister(instr->key()).is(LoadDescriptor::NameRegister()));

  if (instr->hydrogen()->HasVectorAndSlot()) {
    EmitVectorLoadICRegisters<LLoadKeyedGeneric>(instr);
  }

  Handle<Code> ic = CodeFactory::KeyedLoadICInOptimizedCode(
                        isolate(), instr->hydrogen()->language_mode(),
                        instr->hydrogen()->initialization_state()).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoArgumentsElements(LArgumentsElements* instr) {
  Register result = ToRegister(instr->result());

  if (instr->hydrogen()->from_inlined()) {
    __ lea(result, Operand(esp, -2 * kPointerSize));
  } else {
    // Check for arguments adapter frame.
    Label done, adapted;
    __ mov(result, Operand(ebp, StandardFrameConstants::kCallerFPOffset));
    __ mov(result, Operand(result, StandardFrameConstants::kContextOffset));
    __ cmp(Operand(result),
           Immediate(Smi::FromInt(StackFrame::ARGUMENTS_ADAPTOR)));
    __ j(equal, &adapted, Label::kNear);

    // No arguments adaptor frame.
    __ mov(result, Operand(ebp));
    __ jmp(&done, Label::kNear);

    // Arguments adaptor frame present.
    __ bind(&adapted);
    __ mov(result, Operand(ebp, StandardFrameConstants::kCallerFPOffset));

    // Result is the frame pointer for the frame if not adapted and for the real
    // frame below the adaptor frame if adapted.
    __ bind(&done);
  }
}


void LCodeGen::DoArgumentsLength(LArgumentsLength* instr) {
  Operand elem = ToOperand(instr->elements());
  Register result = ToRegister(instr->result());

  Label done;

  // If no arguments adaptor frame the number of arguments is fixed.
  __ cmp(ebp, elem);
  __ mov(result, Immediate(scope()->num_parameters()));
  __ j(equal, &done, Label::kNear);

  // Arguments adaptor frame present. Get argument length from there.
  __ mov(result, Operand(ebp, StandardFrameConstants::kCallerFPOffset));
  __ mov(result, Operand(result,
                         ArgumentsAdaptorFrameConstants::kLengthOffset));
  __ SmiUntag(result);

  // Argument length is in result register.
  __ bind(&done);
}


void LCodeGen::DoWrapReceiver(LWrapReceiver* instr) {
  Register receiver = ToRegister(instr->receiver());
  Register function = ToRegister(instr->function());

  // If the receiver is null or undefined, we have to pass the global
  // object as a receiver to normal functions. Values have to be
  // passed unchanged to builtins and strict-mode functions.
  Label receiver_ok, global_object;
  Label::Distance dist = DeoptEveryNTimes() ? Label::kFar : Label::kNear;
  Register scratch = ToRegister(instr->temp());

  if (!instr->hydrogen()->known_function()) {
    // Do not transform the receiver to object for strict mode
    // functions.
    __ mov(scratch,
           FieldOperand(function, JSFunction::kSharedFunctionInfoOffset));
    __ test_b(FieldOperand(scratch, SharedFunctionInfo::kStrictModeByteOffset),
              1 << SharedFunctionInfo::kStrictModeBitWithinByte);
    __ j(not_equal, &receiver_ok, dist);

    // Do not transform the receiver to object for builtins.
    __ test_b(FieldOperand(scratch, SharedFunctionInfo::kNativeByteOffset),
              1 << SharedFunctionInfo::kNativeBitWithinByte);
    __ j(not_equal, &receiver_ok, dist);
  }

  // Normal function. Replace undefined or null with global receiver.
  __ cmp(receiver, factory()->null_value());
  __ j(equal, &global_object, Label::kNear);
  __ cmp(receiver, factory()->undefined_value());
  __ j(equal, &global_object, Label::kNear);

  // The receiver should be a JS object.
  __ test(receiver, Immediate(kSmiTagMask));
  DeoptimizeIf(equal, instr, Deoptimizer::kSmi);
  __ CmpObjectType(receiver, FIRST_SPEC_OBJECT_TYPE, scratch);
  DeoptimizeIf(below, instr, Deoptimizer::kNotAJavaScriptObject);

  __ jmp(&receiver_ok, Label::kNear);
  __ bind(&global_object);
  __ mov(receiver, FieldOperand(function, JSFunction::kContextOffset));
  const int global_offset = Context::SlotOffset(Context::GLOBAL_OBJECT_INDEX);
  __ mov(receiver, Operand(receiver, global_offset));
  const int proxy_offset = GlobalObject::kGlobalProxyOffset;
  __ mov(receiver, FieldOperand(receiver, proxy_offset));
  __ bind(&receiver_ok);
}


void LCodeGen::DoApplyArguments(LApplyArguments* instr) {
  Register receiver = ToRegister(instr->receiver());
  Register function = ToRegister(instr->function());
  Register length = ToRegister(instr->length());
  Register elements = ToRegister(instr->elements());
  DCHECK(receiver.is(eax));  // Used for parameter count.
  DCHECK(function.is(edi));  // Required by InvokeFunction.
  DCHECK(ToRegister(instr->result()).is(eax));

  // Copy the arguments to this function possibly from the
  // adaptor frame below it.
  const uint32_t kArgumentsLimit = 1 * KB;
  __ cmp(length, kArgumentsLimit);
  DeoptimizeIf(above, instr, Deoptimizer::kTooManyArguments);

  __ push(receiver);
  __ mov(receiver, length);

  // Loop through the arguments pushing them onto the execution
  // stack.
  Label invoke, loop;
  // length is a small non-negative integer, due to the test above.
  __ test(length, Operand(length));
  __ j(zero, &invoke, Label::kNear);
  __ bind(&loop);
  __ push(Operand(elements, length, times_pointer_size, 1 * kPointerSize));
  __ dec(length);
  __ j(not_zero, &loop);

  // Invoke the function.
  __ bind(&invoke);
  DCHECK(instr->HasPointerMap());
  LPointerMap* pointers = instr->pointer_map();
  SafepointGenerator safepoint_generator(
      this, pointers, Safepoint::kLazyDeopt);
  ParameterCount actual(eax);
  __ InvokeFunction(function, actual, CALL_FUNCTION, safepoint_generator);
}


void LCodeGen::DoDebugBreak(LDebugBreak* instr) {
  __ int3();
}


void LCodeGen::DoPushArgument(LPushArgument* instr) {
  LOperand* argument = instr->value();
  EmitPushTaggedOperand(argument);
}


void LCodeGen::DoDrop(LDrop* instr) {
  __ Drop(instr->count());
}


void LCodeGen::DoThisFunction(LThisFunction* instr) {
  Register result = ToRegister(instr->result());
  __ mov(result, Operand(ebp, JavaScriptFrameConstants::kFunctionOffset));
}


void LCodeGen::DoContext(LContext* instr) {
  Register result = ToRegister(instr->result());
  if (info()->IsOptimizing()) {
    __ mov(result, Operand(ebp, StandardFrameConstants::kContextOffset));
  } else {
    // If there is no frame, the context must be in esi.
    DCHECK(result.is(esi));
  }
}


void LCodeGen::DoDeclareGlobals(LDeclareGlobals* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  __ push(esi);  // The context is the first argument.
  __ push(Immediate(instr->hydrogen()->pairs()));
  __ push(Immediate(Smi::FromInt(instr->hydrogen()->flags())));
  CallRuntime(Runtime::kDeclareGlobals, 3, instr);
}


void LCodeGen::CallKnownFunction(Handle<JSFunction> function,
                                 int formal_parameter_count, int arity,
                                 LInstruction* instr) {
  bool dont_adapt_arguments =
      formal_parameter_count == SharedFunctionInfo::kDontAdaptArgumentsSentinel;
  bool can_invoke_directly =
      dont_adapt_arguments || formal_parameter_count == arity;

  Register function_reg = edi;

  if (can_invoke_directly) {
    // Change context.
    __ mov(esi, FieldOperand(function_reg, JSFunction::kContextOffset));

    // Set eax to arguments count if adaption is not needed. Assumes that eax
    // is available to write to at this point.
    if (dont_adapt_arguments) {
      __ mov(eax, arity);
    }

    // Invoke function directly.
    if (function.is_identical_to(info()->closure())) {
      __ CallSelf();
    } else {
      __ call(FieldOperand(function_reg, JSFunction::kCodeEntryOffset));
    }
    RecordSafepointWithLazyDeopt(instr, RECORD_SIMPLE_SAFEPOINT);
  } else {
    // We need to adapt arguments.
    LPointerMap* pointers = instr->pointer_map();
    SafepointGenerator generator(
        this, pointers, Safepoint::kLazyDeopt);
    ParameterCount count(arity);
    ParameterCount expected(formal_parameter_count);
    __ InvokeFunction(function_reg, expected, count, CALL_FUNCTION, generator);
  }
}


void LCodeGen::DoCallWithDescriptor(LCallWithDescriptor* instr) {
  DCHECK(ToRegister(instr->result()).is(eax));

  if (instr->hydrogen()->IsTailCall()) {
    if (NeedsEagerFrame()) __ leave();

    if (instr->target()->IsConstantOperand()) {
      LConstantOperand* target = LConstantOperand::cast(instr->target());
      Handle<Code> code = Handle<Code>::cast(ToHandle(target));
      __ jmp(code, RelocInfo::CODE_TARGET);
    } else {
      DCHECK(instr->target()->IsRegister());
      Register target = ToRegister(instr->target());
      __ add(target, Immediate(Code::kHeaderSize - kHeapObjectTag));
      __ jmp(target);
    }
  } else {
    LPointerMap* pointers = instr->pointer_map();
    SafepointGenerator generator(this, pointers, Safepoint::kLazyDeopt);

    if (instr->target()->IsConstantOperand()) {
      LConstantOperand* target = LConstantOperand::cast(instr->target());
      Handle<Code> code = Handle<Code>::cast(ToHandle(target));
      generator.BeforeCall(__ CallSize(code, RelocInfo::CODE_TARGET));
      __ call(code, RelocInfo::CODE_TARGET);
    } else {
      DCHECK(instr->target()->IsRegister());
      Register target = ToRegister(instr->target());
      generator.BeforeCall(__ CallSize(Operand(target)));
      __ add(target, Immediate(Code::kHeaderSize - kHeapObjectTag));
      __ call(target);
    }
    generator.AfterCall();
  }
}


void LCodeGen::DoCallJSFunction(LCallJSFunction* instr) {
  DCHECK(ToRegister(instr->function()).is(edi));
  DCHECK(ToRegister(instr->result()).is(eax));

  if (instr->hydrogen()->pass_argument_count()) {
    __ mov(eax, instr->arity());
  }

  // Change context.
  __ mov(esi, FieldOperand(edi, JSFunction::kContextOffset));

  bool is_self_call = false;
  if (instr->hydrogen()->function()->IsConstant()) {
    HConstant* fun_const = HConstant::cast(instr->hydrogen()->function());
    Handle<JSFunction> jsfun =
      Handle<JSFunction>::cast(fun_const->handle(isolate()));
    is_self_call = jsfun.is_identical_to(info()->closure());
  }

  if (is_self_call) {
    __ CallSelf();
  } else {
    __ call(FieldOperand(edi, JSFunction::kCodeEntryOffset));
  }

  RecordSafepointWithLazyDeopt(instr, RECORD_SIMPLE_SAFEPOINT);
}


void LCodeGen::DoDeferredMathAbsTaggedHeapNumber(LMathAbs* instr) {
  Register input_reg = ToRegister(instr->value());
  __ cmp(FieldOperand(input_reg, HeapObject::kMapOffset),
         factory()->heap_number_map());
  DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumber);

  Label slow, allocated, done;
  Register tmp = input_reg.is(eax) ? ecx : eax;
  Register tmp2 = tmp.is(ecx) ? edx : input_reg.is(ecx) ? edx : ecx;

  // Preserve the value of all registers.
  PushSafepointRegistersScope scope(this);

  __ mov(tmp, FieldOperand(input_reg, HeapNumber::kExponentOffset));
  // Check the sign of the argument. If the argument is positive, just
  // return it. We do not need to patch the stack since |input| and
  // |result| are the same register and |input| will be restored
  // unchanged by popping safepoint registers.
  __ test(tmp, Immediate(HeapNumber::kSignMask));
  __ j(zero, &done, Label::kNear);

  __ AllocateHeapNumber(tmp, tmp2, no_reg, &slow);
  __ jmp(&allocated, Label::kNear);

  // Slow case: Call the runtime system to do the number allocation.
  __ bind(&slow);
  CallRuntimeFromDeferred(Runtime::kAllocateHeapNumber, 0,
                          instr, instr->context());
  // Set the pointer to the new heap number in tmp.
  if (!tmp.is(eax)) __ mov(tmp, eax);
  // Restore input_reg after call to runtime.
  __ LoadFromSafepointRegisterSlot(input_reg, input_reg);

  __ bind(&allocated);
  __ mov(tmp2, FieldOperand(input_reg, HeapNumber::kExponentOffset));
  __ and_(tmp2, ~HeapNumber::kSignMask);
  __ mov(FieldOperand(tmp, HeapNumber::kExponentOffset), tmp2);
  __ mov(tmp2, FieldOperand(input_reg, HeapNumber::kMantissaOffset));
  __ mov(FieldOperand(tmp, HeapNumber::kMantissaOffset), tmp2);
  __ StoreToSafepointRegisterSlot(input_reg, tmp);

  __ bind(&done);
}


void LCodeGen::EmitIntegerMathAbs(LMathAbs* instr) {
  Register input_reg = ToRegister(instr->value());
  __ test(input_reg, Operand(input_reg));
  Label is_positive;
  __ j(not_sign, &is_positive, Label::kNear);
  __ neg(input_reg);  // Sets flags.
  DeoptimizeIf(negative, instr, Deoptimizer::kOverflow);
  __ bind(&is_positive);
}


void LCodeGen::DoMathAbs(LMathAbs* instr) {
  // Class for deferred case.
  class DeferredMathAbsTaggedHeapNumber final : public LDeferredCode {
   public:
    DeferredMathAbsTaggedHeapNumber(LCodeGen* codegen,
                                    LMathAbs* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override {
      codegen()->DoDeferredMathAbsTaggedHeapNumber(instr_);
    }
    LInstruction* instr() override { return instr_; }

   private:
    LMathAbs* instr_;
  };

  DCHECK(instr->value()->Equals(instr->result()));
  Representation r = instr->hydrogen()->value()->representation();

  if (r.IsDouble()) {
    XMMRegister scratch = double_scratch0();
    XMMRegister input_reg = ToDoubleRegister(instr->value());
    __ xorps(scratch, scratch);
    __ subsd(scratch, input_reg);
    __ andps(input_reg, scratch);
  } else if (r.IsSmiOrInteger32()) {
    EmitIntegerMathAbs(instr);
  } else {  // Tagged case.
    DeferredMathAbsTaggedHeapNumber* deferred =
        new(zone()) DeferredMathAbsTaggedHeapNumber(this, instr);
    Register input_reg = ToRegister(instr->value());
    // Smi check.
    __ JumpIfNotSmi(input_reg, deferred->entry());
    EmitIntegerMathAbs(instr);
    __ bind(deferred->exit());
  }
}


void LCodeGen::DoMathFloor(LMathFloor* instr) {
  XMMRegister xmm_scratch = double_scratch0();
  Register output_reg = ToRegister(instr->result());
  XMMRegister input_reg = ToDoubleRegister(instr->value());

  if (CpuFeatures::IsSupported(SSE4_1)) {
    CpuFeatureScope scope(masm(), SSE4_1);
    if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
      // Deoptimize on negative zero.
      Label non_zero;
      __ xorps(xmm_scratch, xmm_scratch);  // Zero the register.
      __ ucomisd(input_reg, xmm_scratch);
      __ j(not_equal, &non_zero, Label::kNear);
      __ movmskpd(output_reg, input_reg);
      __ test(output_reg, Immediate(1));
      DeoptimizeIf(not_zero, instr, Deoptimizer::kMinusZero);
      __ bind(&non_zero);
    }
    __ roundsd(xmm_scratch, input_reg, kRoundDown);
    __ cvttsd2si(output_reg, Operand(xmm_scratch));
    // Overflow is signalled with minint.
    __ cmp(output_reg, 0x1);
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
  } else {
    Label negative_sign, done;
    // Deoptimize on unordered.
    __ xorps(xmm_scratch, xmm_scratch);  // Zero the register.
    __ ucomisd(input_reg, xmm_scratch);
    DeoptimizeIf(parity_even, instr, Deoptimizer::kNaN);
    __ j(below, &negative_sign, Label::kNear);

    if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
      // Check for negative zero.
      Label positive_sign;
      __ j(above, &positive_sign, Label::kNear);
      __ movmskpd(output_reg, input_reg);
      __ test(output_reg, Immediate(1));
      DeoptimizeIf(not_zero, instr, Deoptimizer::kMinusZero);
      __ Move(output_reg, Immediate(0));
      __ jmp(&done, Label::kNear);
      __ bind(&positive_sign);
    }

    // Use truncating instruction (OK because input is positive).
    __ cvttsd2si(output_reg, Operand(input_reg));
    // Overflow is signalled with minint.
    __ cmp(output_reg, 0x1);
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
    __ jmp(&done, Label::kNear);

    // Non-zero negative reaches here.
    __ bind(&negative_sign);
    // Truncate, then compare and compensate.
    __ cvttsd2si(output_reg, Operand(input_reg));
    __ Cvtsi2sd(xmm_scratch, output_reg);
    __ ucomisd(input_reg, xmm_scratch);
    __ j(equal, &done, Label::kNear);
    __ sub(output_reg, Immediate(1));
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);

    __ bind(&done);
  }
}


void LCodeGen::DoMathRound(LMathRound* instr) {
  Register output_reg = ToRegister(instr->result());
  XMMRegister input_reg = ToDoubleRegister(instr->value());
  XMMRegister xmm_scratch = double_scratch0();
  XMMRegister input_temp = ToDoubleRegister(instr->temp());
  ExternalReference one_half = ExternalReference::address_of_one_half();
  ExternalReference minus_one_half =
      ExternalReference::address_of_minus_one_half();

  Label done, round_to_zero, below_one_half, do_not_compensate;
  Label::Distance dist = DeoptEveryNTimes() ? Label::kFar : Label::kNear;

  __ movsd(xmm_scratch, Operand::StaticVariable(one_half));
  __ ucomisd(xmm_scratch, input_reg);
  __ j(above, &below_one_half, Label::kNear);

  // CVTTSD2SI rounds towards zero, since 0.5 <= x, we use floor(0.5 + x).
  __ addsd(xmm_scratch, input_reg);
  __ cvttsd2si(output_reg, Operand(xmm_scratch));
  // Overflow is signalled with minint.
  __ cmp(output_reg, 0x1);
  DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
  __ jmp(&done, dist);

  __ bind(&below_one_half);
  __ movsd(xmm_scratch, Operand::StaticVariable(minus_one_half));
  __ ucomisd(xmm_scratch, input_reg);
  __ j(below_equal, &round_to_zero, Label::kNear);

  // CVTTSD2SI rounds towards zero, we use ceil(x - (-0.5)) and then
  // compare and compensate.
  __ movaps(input_temp, input_reg);  // Do not alter input_reg.
  __ subsd(input_temp, xmm_scratch);
  __ cvttsd2si(output_reg, Operand(input_temp));
  // Catch minint due to overflow, and to prevent overflow when compensating.
  __ cmp(output_reg, 0x1);
  DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);

  __ Cvtsi2sd(xmm_scratch, output_reg);
  __ ucomisd(xmm_scratch, input_temp);
  __ j(equal, &done, dist);
  __ sub(output_reg, Immediate(1));
  // No overflow because we already ruled out minint.
  __ jmp(&done, dist);

  __ bind(&round_to_zero);
  // We return 0 for the input range [+0, 0.5[, or [-0.5, 0.5[ if
  // we can ignore the difference between a result of -0 and +0.
  if (instr->hydrogen()->CheckFlag(HValue::kBailoutOnMinusZero)) {
    // If the sign is positive, we return +0.
    __ movmskpd(output_reg, input_reg);
    __ test(output_reg, Immediate(1));
    DeoptimizeIf(not_zero, instr, Deoptimizer::kMinusZero);
  }
  __ Move(output_reg, Immediate(0));
  __ bind(&done);
}


void LCodeGen::DoMathFround(LMathFround* instr) {
  XMMRegister input_reg = ToDoubleRegister(instr->value());
  XMMRegister output_reg = ToDoubleRegister(instr->result());
  __ cvtsd2ss(output_reg, input_reg);
  __ cvtss2sd(output_reg, output_reg);
}


void LCodeGen::DoMathSqrt(LMathSqrt* instr) {
  Operand input = ToOperand(instr->value());
  XMMRegister output = ToDoubleRegister(instr->result());
  __ sqrtsd(output, input);
}


void LCodeGen::DoMathPowHalf(LMathPowHalf* instr) {
  XMMRegister xmm_scratch = double_scratch0();
  XMMRegister input_reg = ToDoubleRegister(instr->value());
  Register scratch = ToRegister(instr->temp());
  DCHECK(ToDoubleRegister(instr->result()).is(input_reg));

  // Note that according to ECMA-262 15.8.2.13:
  // Math.pow(-Infinity, 0.5) == Infinity
  // Math.sqrt(-Infinity) == NaN
  Label done, sqrt;
  // Check base for -Infinity.  According to IEEE-754, single-precision
  // -Infinity has the highest 9 bits set and the lowest 23 bits cleared.
  __ mov(scratch, 0xFF800000);
  __ movd(xmm_scratch, scratch);
  __ cvtss2sd(xmm_scratch, xmm_scratch);
  __ ucomisd(input_reg, xmm_scratch);
  // Comparing -Infinity with NaN results in "unordered", which sets the
  // zero flag as if both were equal.  However, it also sets the carry flag.
  __ j(not_equal, &sqrt, Label::kNear);
  __ j(carry, &sqrt, Label::kNear);
  // If input is -Infinity, return Infinity.
  __ xorps(input_reg, input_reg);
  __ subsd(input_reg, xmm_scratch);
  __ jmp(&done, Label::kNear);

  // Square root.
  __ bind(&sqrt);
  __ xorps(xmm_scratch, xmm_scratch);
  __ addsd(input_reg, xmm_scratch);  // Convert -0 to +0.
  __ sqrtsd(input_reg, input_reg);
  __ bind(&done);
}


void LCodeGen::DoPower(LPower* instr) {
  Representation exponent_type = instr->hydrogen()->right()->representation();
  // Having marked this as a call, we can use any registers.
  // Just make sure that the input/output registers are the expected ones.
  Register tagged_exponent = MathPowTaggedDescriptor::exponent();
  DCHECK(!instr->right()->IsDoubleRegister() ||
         ToDoubleRegister(instr->right()).is(xmm1));
  DCHECK(!instr->right()->IsRegister() ||
         ToRegister(instr->right()).is(tagged_exponent));
  DCHECK(ToDoubleRegister(instr->left()).is(xmm2));
  DCHECK(ToDoubleRegister(instr->result()).is(xmm3));

  if (exponent_type.IsSmi()) {
    MathPowStub stub(isolate(), MathPowStub::TAGGED);
    __ CallStub(&stub);
  } else if (exponent_type.IsTagged()) {
    Label no_deopt;
    __ JumpIfSmi(tagged_exponent, &no_deopt);
    DCHECK(!ecx.is(tagged_exponent));
    __ CmpObjectType(tagged_exponent, HEAP_NUMBER_TYPE, ecx);
    DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumber);
    __ bind(&no_deopt);
    MathPowStub stub(isolate(), MathPowStub::TAGGED);
    __ CallStub(&stub);
  } else if (exponent_type.IsInteger32()) {
    MathPowStub stub(isolate(), MathPowStub::INTEGER);
    __ CallStub(&stub);
  } else {
    DCHECK(exponent_type.IsDouble());
    MathPowStub stub(isolate(), MathPowStub::DOUBLE);
    __ CallStub(&stub);
  }
}


void LCodeGen::DoMathLog(LMathLog* instr) {
  DCHECK(instr->value()->Equals(instr->result()));
  XMMRegister input_reg = ToDoubleRegister(instr->value());
  XMMRegister xmm_scratch = double_scratch0();
  Label positive, done, zero;
  __ xorps(xmm_scratch, xmm_scratch);
  __ ucomisd(input_reg, xmm_scratch);
  __ j(above, &positive, Label::kNear);
  __ j(not_carry, &zero, Label::kNear);
  __ pcmpeqd(input_reg, input_reg);
  __ jmp(&done, Label::kNear);
  __ bind(&zero);
  ExternalReference ninf =
      ExternalReference::address_of_negative_infinity();
  __ movsd(input_reg, Operand::StaticVariable(ninf));
  __ jmp(&done, Label::kNear);
  __ bind(&positive);
  __ fldln2();
  __ sub(Operand(esp), Immediate(kDoubleSize));
  __ movsd(Operand(esp, 0), input_reg);
  __ fld_d(Operand(esp, 0));
  __ fyl2x();
  __ fstp_d(Operand(esp, 0));
  __ movsd(input_reg, Operand(esp, 0));
  __ add(Operand(esp), Immediate(kDoubleSize));
  __ bind(&done);
}


void LCodeGen::DoMathClz32(LMathClz32* instr) {
  Register input = ToRegister(instr->value());
  Register result = ToRegister(instr->result());

  __ Lzcnt(result, input);
}


void LCodeGen::DoMathExp(LMathExp* instr) {
  XMMRegister input = ToDoubleRegister(instr->value());
  XMMRegister result = ToDoubleRegister(instr->result());
  XMMRegister temp0 = double_scratch0();
  Register temp1 = ToRegister(instr->temp1());
  Register temp2 = ToRegister(instr->temp2());

  MathExpGenerator::EmitMathExp(masm(), input, result, temp0, temp1, temp2);
}


void LCodeGen::DoInvokeFunction(LInvokeFunction* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->function()).is(edi));
  DCHECK(instr->HasPointerMap());

  Handle<JSFunction> known_function = instr->hydrogen()->known_function();
  if (known_function.is_null()) {
    LPointerMap* pointers = instr->pointer_map();
    SafepointGenerator generator(
        this, pointers, Safepoint::kLazyDeopt);
    ParameterCount count(instr->arity());
    __ InvokeFunction(edi, count, CALL_FUNCTION, generator);
  } else {
    CallKnownFunction(known_function,
                      instr->hydrogen()->formal_parameter_count(),
                      instr->arity(), instr);
  }
}


void LCodeGen::DoCallFunction(LCallFunction* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->function()).is(edi));
  DCHECK(ToRegister(instr->result()).is(eax));

  int arity = instr->arity();
  CallFunctionFlags flags = instr->hydrogen()->function_flags();
  if (instr->hydrogen()->HasVectorAndSlot()) {
    Register slot_register = ToRegister(instr->temp_slot());
    Register vector_register = ToRegister(instr->temp_vector());
    DCHECK(slot_register.is(edx));
    DCHECK(vector_register.is(ebx));

    AllowDeferredHandleDereference vector_structure_check;
    Handle<TypeFeedbackVector> vector = instr->hydrogen()->feedback_vector();
    int index = vector->GetIndex(instr->hydrogen()->slot());

    __ mov(vector_register, vector);
    __ mov(slot_register, Immediate(Smi::FromInt(index)));

    CallICState::CallType call_type =
        (flags & CALL_AS_METHOD) ? CallICState::METHOD : CallICState::FUNCTION;

    Handle<Code> ic =
        CodeFactory::CallICInOptimizedCode(isolate(), arity, call_type).code();
    CallCode(ic, RelocInfo::CODE_TARGET, instr);
  } else {
    CallFunctionStub stub(isolate(), arity, flags);
    CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
  }
}


void LCodeGen::DoCallNew(LCallNew* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->constructor()).is(edi));
  DCHECK(ToRegister(instr->result()).is(eax));

  // No cell in ebx for construct type feedback in optimized code
  __ mov(ebx, isolate()->factory()->undefined_value());
  CallConstructStub stub(isolate(), NO_CALL_CONSTRUCTOR_FLAGS);
  __ Move(eax, Immediate(instr->arity()));
  CallCode(stub.GetCode(), RelocInfo::CONSTRUCT_CALL, instr);
}


void LCodeGen::DoCallNewArray(LCallNewArray* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->constructor()).is(edi));
  DCHECK(ToRegister(instr->result()).is(eax));

  __ Move(eax, Immediate(instr->arity()));
  if (instr->arity() == 1) {
    // We only need the allocation site for the case we have a length argument.
    // The case may bail out to the runtime, which will determine the correct
    // elements kind with the site.
    __ mov(ebx, instr->hydrogen()->site());
  } else {
    __ mov(ebx, isolate()->factory()->undefined_value());
  }

  ElementsKind kind = instr->hydrogen()->elements_kind();
  AllocationSiteOverrideMode override_mode =
      (AllocationSite::GetMode(kind) == TRACK_ALLOCATION_SITE)
          ? DISABLE_ALLOCATION_SITES
          : DONT_OVERRIDE;

  if (instr->arity() == 0) {
    ArrayNoArgumentConstructorStub stub(isolate(), kind, override_mode);
    CallCode(stub.GetCode(), RelocInfo::CONSTRUCT_CALL, instr);
  } else if (instr->arity() == 1) {
    Label done;
    if (IsFastPackedElementsKind(kind)) {
      Label packed_case;
      // We might need a change here
      // look at the first argument
      __ mov(ecx, Operand(esp, 0));
      __ test(ecx, ecx);
      __ j(zero, &packed_case, Label::kNear);

      ElementsKind holey_kind = GetHoleyElementsKind(kind);
      ArraySingleArgumentConstructorStub stub(isolate(),
                                              holey_kind,
                                              override_mode);
      CallCode(stub.GetCode(), RelocInfo::CONSTRUCT_CALL, instr);
      __ jmp(&done, Label::kNear);
      __ bind(&packed_case);
    }

    ArraySingleArgumentConstructorStub stub(isolate(), kind, override_mode);
    CallCode(stub.GetCode(), RelocInfo::CONSTRUCT_CALL, instr);
    __ bind(&done);
  } else {
    ArrayNArgumentsConstructorStub stub(isolate(), kind, override_mode);
    CallCode(stub.GetCode(), RelocInfo::CONSTRUCT_CALL, instr);
  }
}


void LCodeGen::DoCallRuntime(LCallRuntime* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  CallRuntime(instr->function(), instr->arity(), instr, instr->save_doubles());
}


void LCodeGen::DoStoreCodeEntry(LStoreCodeEntry* instr) {
  Register function = ToRegister(instr->function());
  Register code_object = ToRegister(instr->code_object());
  __ lea(code_object, FieldOperand(code_object, Code::kHeaderSize));
  __ mov(FieldOperand(function, JSFunction::kCodeEntryOffset), code_object);
}


void LCodeGen::DoInnerAllocatedObject(LInnerAllocatedObject* instr) {
  Register result = ToRegister(instr->result());
  Register base = ToRegister(instr->base_object());
  if (instr->offset()->IsConstantOperand()) {
    LConstantOperand* offset = LConstantOperand::cast(instr->offset());
    __ lea(result, Operand(base, ToInteger32(offset)));
  } else {
    Register offset = ToRegister(instr->offset());
    __ lea(result, Operand(base, offset, times_1, 0));
  }
}


void LCodeGen::DoStoreNamedField(LStoreNamedField* instr) {
  Representation representation = instr->hydrogen()->field_representation();

  HObjectAccess access = instr->hydrogen()->access();
  int offset = access.offset();

  if (access.IsExternalMemory()) {
    DCHECK(!instr->hydrogen()->NeedsWriteBarrier());
    MemOperand operand = instr->object()->IsConstantOperand()
        ? MemOperand::StaticVariable(
            ToExternalReference(LConstantOperand::cast(instr->object())))
        : MemOperand(ToRegister(instr->object()), offset);
    if (instr->value()->IsConstantOperand()) {
      LConstantOperand* operand_value = LConstantOperand::cast(instr->value());
      __ mov(operand, Immediate(ToInteger32(operand_value)));
    } else {
      Register value = ToRegister(instr->value());
      __ Store(value, operand, representation);
    }
    return;
  }

  Register object = ToRegister(instr->object());
  __ AssertNotSmi(object);

  DCHECK(!representation.IsSmi() ||
         !instr->value()->IsConstantOperand() ||
         IsSmi(LConstantOperand::cast(instr->value())));
  if (representation.IsDouble()) {
    DCHECK(access.IsInobject());
    DCHECK(!instr->hydrogen()->has_transition());
    DCHECK(!instr->hydrogen()->NeedsWriteBarrier());
    XMMRegister value = ToDoubleRegister(instr->value());
    __ movsd(FieldOperand(object, offset), value);
    return;
  }

  if (instr->hydrogen()->has_transition()) {
    Handle<Map> transition = instr->hydrogen()->transition_map();
    AddDeprecationDependency(transition);
    __ mov(FieldOperand(object, HeapObject::kMapOffset), transition);
    if (instr->hydrogen()->NeedsWriteBarrierForMap()) {
      Register temp = ToRegister(instr->temp());
      Register temp_map = ToRegister(instr->temp_map());
      // Update the write barrier for the map field.
      __ RecordWriteForMap(object, transition, temp_map, temp, kSaveFPRegs);
    }
  }

  // Do the store.
  Register write_register = object;
  if (!access.IsInobject()) {
    write_register = ToRegister(instr->temp());
    __ mov(write_register, FieldOperand(object, JSObject::kPropertiesOffset));
  }

  MemOperand operand = FieldOperand(write_register, offset);
  if (instr->value()->IsConstantOperand()) {
    LConstantOperand* operand_value = LConstantOperand::cast(instr->value());
    if (operand_value->IsRegister()) {
      Register value = ToRegister(operand_value);
      __ Store(value, operand, representation);
    } else if (representation.IsInteger32()) {
      Immediate immediate = ToImmediate(operand_value, representation);
      DCHECK(!instr->hydrogen()->NeedsWriteBarrier());
      __ mov(operand, immediate);
    } else {
      Handle<Object> handle_value = ToHandle(operand_value);
      DCHECK(!instr->hydrogen()->NeedsWriteBarrier());
      __ mov(operand, handle_value);
    }
  } else {
    Register value = ToRegister(instr->value());
    __ Store(value, operand, representation);
  }

  if (instr->hydrogen()->NeedsWriteBarrier()) {
    Register value = ToRegister(instr->value());
    Register temp = access.IsInobject() ? ToRegister(instr->temp()) : object;
    // Update the write barrier for the object for in-object properties.
    __ RecordWriteField(write_register,
                        offset,
                        value,
                        temp,
                        kSaveFPRegs,
                        EMIT_REMEMBERED_SET,
                        instr->hydrogen()->SmiCheckForWriteBarrier(),
                        instr->hydrogen()->PointersToHereCheckForValue());
  }
}


void LCodeGen::DoStoreNamedGeneric(LStoreNamedGeneric* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->object()).is(StoreDescriptor::ReceiverRegister()));
  DCHECK(ToRegister(instr->value()).is(StoreDescriptor::ValueRegister()));

  if (instr->hydrogen()->HasVectorAndSlot()) {
    EmitVectorStoreICRegisters<LStoreNamedGeneric>(instr);
  }

  __ mov(StoreDescriptor::NameRegister(), instr->name());
  Handle<Code> ic = CodeFactory::StoreICInOptimizedCode(
                        isolate(), instr->language_mode(),
                        instr->hydrogen()->initialization_state()).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoBoundsCheck(LBoundsCheck* instr) {
  Condition cc = instr->hydrogen()->allow_equality() ? above : above_equal;
  BuiltinFunctionId op = instr->hydrogen()->op();
  if (IsSIMD128LoadStoreOp(op)) {
    cc = above;
    Register index_in_bytes = ToRegister(instr->temp0());
    Register length_in_bytes = ToRegister(instr->temp1());
    if (instr->index()->IsConstantOperand())
      __ mov(index_in_bytes, ToImmediate(LConstantOperand::cast(instr->index()),
          instr->hydrogen()->index()->representation()));
    else
      __ mov(index_in_bytes, ToOperand(instr->index()));
    int index_shift_size =
        ElementsKindToShiftSize(instr->hydrogen()->element_kind());
    if (instr->hydrogen()->index()->representation().IsSmi())
      index_shift_size -= kSmiTagSize;
    DCHECK(index_shift_size >= 0);
    if (index_shift_size > 0)
      __ shl(index_in_bytes, index_shift_size);
    int bytes = GetSIMD128LoadStoreBytes(op);
    __ add(index_in_bytes, Immediate(bytes));
    if (instr->length()->IsConstantOperand())
      __ mov(length_in_bytes,
             ToImmediate(LConstantOperand::cast(instr->length()),
      instr->hydrogen()->length()->representation()));
    else
      __ mov(length_in_bytes, ToOperand(instr->length()));
    int length_shift_size =
      ElementsKindToShiftSize(instr->hydrogen()->element_kind());
    if (instr->hydrogen()->length()->representation().IsSmi())
      length_shift_size -= kSmiTagSize;
    DCHECK(length_shift_size >= 0);
    if (length_shift_size > 0)
      __ shl(length_in_bytes, length_shift_size);
    __ cmp(index_in_bytes, length_in_bytes);
  } else {
    if (instr->index()->IsConstantOperand()) {
      __ cmp(ToOperand(instr->length()),
        ToImmediate(LConstantOperand::cast(instr->index()),
        instr->hydrogen()->length()->representation()));
      cc = CommuteCondition(cc);
    } else if (instr->length()->IsConstantOperand()) {
      __ cmp(ToOperand(instr->index()),
        ToImmediate(LConstantOperand::cast(instr->length()),
        instr->hydrogen()->index()->representation()));
    } else {
      __ cmp(ToRegister(instr->index()), ToOperand(instr->length()));
    }
  }
  if (FLAG_debug_code && instr->hydrogen()->skip_check()) {
    Label done;
    __ j(NegateCondition(cc), &done, Label::kNear);
    __ int3();
    __ bind(&done);
  } else {
    DeoptimizeIf(cc, instr, Deoptimizer::kOutOfBounds);
  }
}


void LCodeGen::DoStoreKeyedExternalArray(LStoreKeyed* instr) {
  ElementsKind elements_kind = instr->elements_kind();
  LOperand* key = instr->key();
  if (!key->IsConstantOperand() &&
      ExternalArrayOpRequiresTemp(instr->hydrogen()->key()->representation(),
                                  elements_kind)) {
    HandleExternalArrayOpRequiresTemp(
        key, instr->hydrogen()->key()->representation(), elements_kind);
  }

  Operand operand(BuildFastArrayOperand(
      instr->elements(),
      key,
      instr->hydrogen()->key()->representation(),
      elements_kind,
      instr->base_offset()));
  BuiltinFunctionId op = instr->hydrogen()->op();
  if (IsSIMD128LoadStoreOp(op)) {
    if (GetSIMD128LoadStoreBytes(op) == 16) {
      __ movups(operand, ToSIMD128Register(instr->value()));
    } else if (GetSIMD128LoadStoreBytes(op) == 4) {
      __ movss(operand, ToSIMD128Register(instr->value()));
    } else if (GetSIMD128LoadStoreBytes(op) == 8) {
      __ movq(operand, ToSIMD128Register(instr->value()));
    } else if (GetSIMD128LoadStoreBytes(op) == 12) {
      XMMRegister value(ToSIMD128Register(instr->value()));
      XMMRegister xmm_scratch = double_scratch0();
      __ movq(operand, value);
      Operand operand2(BuildFastArrayOperand(
          instr->elements(),
          key,
          instr->hydrogen()->key()->representation(),
          elements_kind,
          instr->base_offset() + 8));
      __ movhlps(xmm_scratch, value);
      __ movss(operand2, xmm_scratch);
    }
  } else if (elements_kind == EXTERNAL_FLOAT32_ELEMENTS ||
      elements_kind == FLOAT32_ELEMENTS) {
    XMMRegister xmm_scratch = double_scratch0();
    __ cvtsd2ss(xmm_scratch, ToDoubleRegister(instr->value()));
    __ movss(operand, xmm_scratch);
  } else if (elements_kind == EXTERNAL_FLOAT64_ELEMENTS ||
             elements_kind == FLOAT64_ELEMENTS) {
    __ movsd(operand, ToDoubleRegister(instr->value()));
  } else if (IsSIMD128ElementsKind(elements_kind)) {
    __ movups(operand, ToSIMD128Register(instr->value()));
  } else {
    Register value = ToRegister(instr->value());
    switch (elements_kind) {
      case EXTERNAL_UINT8_CLAMPED_ELEMENTS:
      case EXTERNAL_UINT8_ELEMENTS:
      case EXTERNAL_INT8_ELEMENTS:
      case UINT8_ELEMENTS:
      case INT8_ELEMENTS:
      case UINT8_CLAMPED_ELEMENTS:
        __ mov_b(operand, value);
        break;
      case EXTERNAL_INT16_ELEMENTS:
      case EXTERNAL_UINT16_ELEMENTS:
      case UINT16_ELEMENTS:
      case INT16_ELEMENTS:
        __ mov_w(operand, value);
        break;
      case EXTERNAL_INT32_ELEMENTS:
      case EXTERNAL_UINT32_ELEMENTS:
      case UINT32_ELEMENTS:
      case INT32_ELEMENTS:
        __ mov(operand, value);
        break;
      case EXTERNAL_FLOAT32_ELEMENTS:
      case EXTERNAL_FLOAT64_ELEMENTS:
      case FLOAT32_ELEMENTS:
      case FLOAT64_ELEMENTS:
      case FAST_SMI_ELEMENTS:
      case FAST_ELEMENTS:
      case FAST_DOUBLE_ELEMENTS:
      case FAST_HOLEY_SMI_ELEMENTS:
      case FAST_HOLEY_ELEMENTS:
      case FAST_HOLEY_DOUBLE_ELEMENTS:
      case DICTIONARY_ELEMENTS:
      case FAST_SLOPPY_ARGUMENTS_ELEMENTS:
      case SLOW_SLOPPY_ARGUMENTS_ELEMENTS:
        UNREACHABLE();
        break;
    }
  }
}


void LCodeGen::DoStoreKeyedFixedDoubleArray(LStoreKeyed* instr) {
  Operand double_store_operand = BuildFastArrayOperand(
      instr->elements(),
      instr->key(),
      instr->hydrogen()->key()->representation(),
      FAST_DOUBLE_ELEMENTS,
      instr->base_offset());

  XMMRegister value = ToDoubleRegister(instr->value());

  if (instr->NeedsCanonicalization()) {
    XMMRegister xmm_scratch = double_scratch0();
    // Turn potential sNaN value into qNaN.
    __ xorps(xmm_scratch, xmm_scratch);
    __ subsd(value, xmm_scratch);
  }

  __ movsd(double_store_operand, value);
}


void LCodeGen::DoStoreKeyedFixedArray(LStoreKeyed* instr) {
  Register elements = ToRegister(instr->elements());
  Register key = instr->key()->IsRegister() ? ToRegister(instr->key()) : no_reg;

  Operand operand = BuildFastArrayOperand(
      instr->elements(),
      instr->key(),
      instr->hydrogen()->key()->representation(),
      FAST_ELEMENTS,
      instr->base_offset());
  if (instr->value()->IsRegister()) {
    __ mov(operand, ToRegister(instr->value()));
  } else {
    LConstantOperand* operand_value = LConstantOperand::cast(instr->value());
    if (IsSmi(operand_value)) {
      Immediate immediate = ToImmediate(operand_value, Representation::Smi());
      __ mov(operand, immediate);
    } else {
      DCHECK(!IsInteger32(operand_value));
      Handle<Object> handle_value = ToHandle(operand_value);
      __ mov(operand, handle_value);
    }
  }

  if (instr->hydrogen()->NeedsWriteBarrier()) {
    DCHECK(instr->value()->IsRegister());
    Register value = ToRegister(instr->value());
    DCHECK(!instr->key()->IsConstantOperand());
    SmiCheck check_needed =
        instr->hydrogen()->value()->type().IsHeapObject()
          ? OMIT_SMI_CHECK : INLINE_SMI_CHECK;
    // Compute address of modified element and store it into key register.
    __ lea(key, operand);
    __ RecordWrite(elements,
                   key,
                   value,
                   kSaveFPRegs,
                   EMIT_REMEMBERED_SET,
                   check_needed,
                   instr->hydrogen()->PointersToHereCheckForValue());
  }
}


void LCodeGen::DoStoreKeyed(LStoreKeyed* instr) {
  // By cases...external, fast-double, fast
  if (instr->is_typed_elements()) {
    DoStoreKeyedExternalArray(instr);
  } else if (instr->hydrogen()->value()->representation().IsDouble()) {
    DoStoreKeyedFixedDoubleArray(instr);
  } else {
    DoStoreKeyedFixedArray(instr);
  }
}


void LCodeGen::DoStoreKeyedGeneric(LStoreKeyedGeneric* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->object()).is(StoreDescriptor::ReceiverRegister()));
  DCHECK(ToRegister(instr->key()).is(StoreDescriptor::NameRegister()));
  DCHECK(ToRegister(instr->value()).is(StoreDescriptor::ValueRegister()));

  if (instr->hydrogen()->HasVectorAndSlot()) {
    EmitVectorStoreICRegisters<LStoreKeyedGeneric>(instr);
  }

  Handle<Code> ic = CodeFactory::KeyedStoreICInOptimizedCode(
                        isolate(), instr->language_mode(),
                        instr->hydrogen()->initialization_state()).code();
  CallCode(ic, RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoTrapAllocationMemento(LTrapAllocationMemento* instr) {
  Register object = ToRegister(instr->object());
  Register temp = ToRegister(instr->temp());
  Label no_memento_found;
  __ TestJSArrayForAllocationMemento(object, temp, &no_memento_found);
  DeoptimizeIf(equal, instr, Deoptimizer::kMementoFound);
  __ bind(&no_memento_found);
}


void LCodeGen::DoMaybeGrowElements(LMaybeGrowElements* instr) {
  class DeferredMaybeGrowElements final : public LDeferredCode {
   public:
    DeferredMaybeGrowElements(LCodeGen* codegen, LMaybeGrowElements* instr)
        : LDeferredCode(codegen), instr_(instr) {}
    void Generate() override { codegen()->DoDeferredMaybeGrowElements(instr_); }
    LInstruction* instr() override { return instr_; }

   private:
    LMaybeGrowElements* instr_;
  };

  Register result = eax;
  DeferredMaybeGrowElements* deferred =
      new (zone()) DeferredMaybeGrowElements(this, instr);
  LOperand* key = instr->key();
  LOperand* current_capacity = instr->current_capacity();

  DCHECK(instr->hydrogen()->key()->representation().IsInteger32());
  DCHECK(instr->hydrogen()->current_capacity()->representation().IsInteger32());
  DCHECK(key->IsConstantOperand() || key->IsRegister());
  DCHECK(current_capacity->IsConstantOperand() ||
         current_capacity->IsRegister());

  if (key->IsConstantOperand() && current_capacity->IsConstantOperand()) {
    int32_t constant_key = ToInteger32(LConstantOperand::cast(key));
    int32_t constant_capacity =
        ToInteger32(LConstantOperand::cast(current_capacity));
    if (constant_key >= constant_capacity) {
      // Deferred case.
      __ jmp(deferred->entry());
    }
  } else if (key->IsConstantOperand()) {
    int32_t constant_key = ToInteger32(LConstantOperand::cast(key));
    __ cmp(ToOperand(current_capacity), Immediate(constant_key));
    __ j(less_equal, deferred->entry());
  } else if (current_capacity->IsConstantOperand()) {
    int32_t constant_capacity =
        ToInteger32(LConstantOperand::cast(current_capacity));
    __ cmp(ToRegister(key), Immediate(constant_capacity));
    __ j(greater_equal, deferred->entry());
  } else {
    __ cmp(ToRegister(key), ToRegister(current_capacity));
    __ j(greater_equal, deferred->entry());
  }

  __ mov(result, ToOperand(instr->elements()));
  __ bind(deferred->exit());
}


void LCodeGen::DoDeferredMaybeGrowElements(LMaybeGrowElements* instr) {
  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  Register result = eax;
  __ Move(result, Immediate(0));

  // We have to call a stub.
  {
    PushSafepointRegistersScope scope(this);
    if (instr->object()->IsRegister()) {
      __ Move(result, ToRegister(instr->object()));
    } else {
      __ mov(result, ToOperand(instr->object()));
    }

    LOperand* key = instr->key();
    if (key->IsConstantOperand()) {
      __ mov(ebx, ToImmediate(key, Representation::Smi()));
    } else {
      __ Move(ebx, ToRegister(key));
      __ SmiTag(ebx);
    }

    GrowArrayElementsStub stub(isolate(), instr->hydrogen()->is_js_array(),
                               instr->hydrogen()->kind());
    __ CallStub(&stub);
    RecordSafepointWithLazyDeopt(
        instr, RECORD_SAFEPOINT_WITH_REGISTERS_AND_NO_ARGUMENTS);
    __ StoreToSafepointRegisterSlot(result, result);
  }

  // Deopt on smi, which means the elements array changed to dictionary mode.
  __ test(result, Immediate(kSmiTagMask));
  DeoptimizeIf(equal, instr, Deoptimizer::kSmi);
}


void LCodeGen::DoTransitionElementsKind(LTransitionElementsKind* instr) {
  Register object_reg = ToRegister(instr->object());

  Handle<Map> from_map = instr->original_map();
  Handle<Map> to_map = instr->transitioned_map();
  ElementsKind from_kind = instr->from_kind();
  ElementsKind to_kind = instr->to_kind();

  Label not_applicable;
  bool is_simple_map_transition =
      IsSimpleMapChangeTransition(from_kind, to_kind);
  Label::Distance branch_distance =
      is_simple_map_transition ? Label::kNear : Label::kFar;
  __ cmp(FieldOperand(object_reg, HeapObject::kMapOffset), from_map);
  __ j(not_equal, &not_applicable, branch_distance);
  if (is_simple_map_transition) {
    Register new_map_reg = ToRegister(instr->new_map_temp());
    __ mov(FieldOperand(object_reg, HeapObject::kMapOffset),
           Immediate(to_map));
    // Write barrier.
    DCHECK_NOT_NULL(instr->temp());
    __ RecordWriteForMap(object_reg, to_map, new_map_reg,
                         ToRegister(instr->temp()),
                         kDontSaveFPRegs);
  } else {
    DCHECK(ToRegister(instr->context()).is(esi));
    DCHECK(object_reg.is(eax));
    PushSafepointRegistersScope scope(this);
    __ mov(ebx, to_map);
    bool is_js_array = from_map->instance_type() == JS_ARRAY_TYPE;
    TransitionElementsKindStub stub(isolate(), from_kind, to_kind, is_js_array);
    __ CallStub(&stub);
    RecordSafepointWithLazyDeopt(instr,
        RECORD_SAFEPOINT_WITH_REGISTERS_AND_NO_ARGUMENTS);
  }
  __ bind(&not_applicable);
}


void LCodeGen::DoStringCharCodeAt(LStringCharCodeAt* instr) {
  class DeferredStringCharCodeAt final : public LDeferredCode {
   public:
    DeferredStringCharCodeAt(LCodeGen* codegen,
                             LStringCharCodeAt* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override { codegen()->DoDeferredStringCharCodeAt(instr_); }
    LInstruction* instr() override { return instr_; }

   private:
    LStringCharCodeAt* instr_;
  };

  DeferredStringCharCodeAt* deferred =
      new(zone()) DeferredStringCharCodeAt(this, instr);

  StringCharLoadGenerator::Generate(masm(),
                                    factory(),
                                    ToRegister(instr->string()),
                                    ToRegister(instr->index()),
                                    ToRegister(instr->result()),
                                    deferred->entry());
  __ bind(deferred->exit());
}


void LCodeGen::DoDeferredStringCharCodeAt(LStringCharCodeAt* instr) {
  Register string = ToRegister(instr->string());
  Register result = ToRegister(instr->result());

  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  __ Move(result, Immediate(0));

  PushSafepointRegistersScope scope(this);
  __ push(string);
  // Push the index as a smi. This is safe because of the checks in
  // DoStringCharCodeAt above.
  STATIC_ASSERT(String::kMaxLength <= Smi::kMaxValue);
  if (instr->index()->IsConstantOperand()) {
    Immediate immediate = ToImmediate(LConstantOperand::cast(instr->index()),
                                      Representation::Smi());
    __ push(immediate);
  } else {
    Register index = ToRegister(instr->index());
    __ SmiTag(index);
    __ push(index);
  }
  CallRuntimeFromDeferred(Runtime::kStringCharCodeAtRT, 2,
                          instr, instr->context());
  __ AssertSmi(eax);
  __ SmiUntag(eax);
  __ StoreToSafepointRegisterSlot(result, eax);
}


void LCodeGen::DoStringCharFromCode(LStringCharFromCode* instr) {
  class DeferredStringCharFromCode final : public LDeferredCode {
   public:
    DeferredStringCharFromCode(LCodeGen* codegen,
                               LStringCharFromCode* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override {
      codegen()->DoDeferredStringCharFromCode(instr_);
    }
    LInstruction* instr() override { return instr_; }

   private:
    LStringCharFromCode* instr_;
  };

  DeferredStringCharFromCode* deferred =
      new(zone()) DeferredStringCharFromCode(this, instr);

  DCHECK(instr->hydrogen()->value()->representation().IsInteger32());
  Register char_code = ToRegister(instr->char_code());
  Register result = ToRegister(instr->result());
  DCHECK(!char_code.is(result));

  __ cmp(char_code, String::kMaxOneByteCharCode);
  __ j(above, deferred->entry());
  __ Move(result, Immediate(factory()->single_character_string_cache()));
  __ mov(result, FieldOperand(result,
                              char_code, times_pointer_size,
                              FixedArray::kHeaderSize));
  __ cmp(result, factory()->undefined_value());
  __ j(equal, deferred->entry());
  __ bind(deferred->exit());
}


void LCodeGen::DoDeferredStringCharFromCode(LStringCharFromCode* instr) {
  Register char_code = ToRegister(instr->char_code());
  Register result = ToRegister(instr->result());

  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  __ Move(result, Immediate(0));

  PushSafepointRegistersScope scope(this);
  __ SmiTag(char_code);
  __ push(char_code);
  CallRuntimeFromDeferred(Runtime::kCharFromCode, 1, instr, instr->context());
  __ StoreToSafepointRegisterSlot(result, eax);
}


void LCodeGen::DoStringAdd(LStringAdd* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->left()).is(edx));
  DCHECK(ToRegister(instr->right()).is(eax));
  StringAddStub stub(isolate(),
                     instr->hydrogen()->flags(),
                     instr->hydrogen()->pretenure_flag());
  CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
}


void LCodeGen::DoInteger32ToDouble(LInteger32ToDouble* instr) {
  LOperand* input = instr->value();
  LOperand* output = instr->result();
  DCHECK(input->IsRegister() || input->IsStackSlot());
  DCHECK(output->IsDoubleRegister());
  __ Cvtsi2sd(ToDoubleRegister(output), ToOperand(input));
}


void LCodeGen::DoUint32ToDouble(LUint32ToDouble* instr) {
  LOperand* input = instr->value();
  LOperand* output = instr->result();
  __ LoadUint32(ToDoubleRegister(output), ToRegister(input));
}


void LCodeGen::DoNumberTagI(LNumberTagI* instr) {
  class DeferredNumberTagI final : public LDeferredCode {
   public:
    DeferredNumberTagI(LCodeGen* codegen,
                       LNumberTagI* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override {
      codegen()->DoDeferredNumberTagIU(
          instr_, instr_->value(), instr_->temp(), SIGNED_INT32);
    }
    LInstruction* instr() override { return instr_; }

   private:
    LNumberTagI* instr_;
  };

  LOperand* input = instr->value();
  DCHECK(input->IsRegister() && input->Equals(instr->result()));
  Register reg = ToRegister(input);

  DeferredNumberTagI* deferred =
      new(zone()) DeferredNumberTagI(this, instr);
  __ SmiTag(reg);
  __ j(overflow, deferred->entry());
  __ bind(deferred->exit());
}


void LCodeGen::DoNumberTagU(LNumberTagU* instr) {
  class DeferredNumberTagU final : public LDeferredCode {
   public:
    DeferredNumberTagU(LCodeGen* codegen, LNumberTagU* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override {
      codegen()->DoDeferredNumberTagIU(
          instr_, instr_->value(), instr_->temp(), UNSIGNED_INT32);
    }
    LInstruction* instr() override { return instr_; }

   private:
    LNumberTagU* instr_;
  };

  LOperand* input = instr->value();
  DCHECK(input->IsRegister() && input->Equals(instr->result()));
  Register reg = ToRegister(input);

  DeferredNumberTagU* deferred =
      new(zone()) DeferredNumberTagU(this, instr);
  __ cmp(reg, Immediate(Smi::kMaxValue));
  __ j(above, deferred->entry());
  __ SmiTag(reg);
  __ bind(deferred->exit());
}


void LCodeGen::DoDeferredNumberTagIU(LInstruction* instr,
                                     LOperand* value,
                                     LOperand* temp,
                                     IntegerSignedness signedness) {
  Label done, slow;
  Register reg = ToRegister(value);
  Register tmp = ToRegister(temp);
  XMMRegister xmm_scratch = double_scratch0();

  if (signedness == SIGNED_INT32) {
    // There was overflow, so bits 30 and 31 of the original integer
    // disagree. Try to allocate a heap number in new space and store
    // the value in there. If that fails, call the runtime system.
    __ SmiUntag(reg);
    __ xor_(reg, 0x80000000);
    __ Cvtsi2sd(xmm_scratch, Operand(reg));
  } else {
    __ LoadUint32(xmm_scratch, reg);
  }

  if (FLAG_inline_new) {
    __ AllocateHeapNumber(reg, tmp, no_reg, &slow);
    __ jmp(&done, Label::kNear);
  }

  // Slow case: Call the runtime system to do the number allocation.
  __ bind(&slow);
  {
    // TODO(3095996): Put a valid pointer value in the stack slot where the
    // result register is stored, as this register is in the pointer map, but
    // contains an integer value.
    __ Move(reg, Immediate(0));

    // Preserve the value of all registers.
    PushSafepointRegistersScope scope(this);

    // NumberTagI and NumberTagD use the context from the frame, rather than
    // the environment's HContext or HInlinedContext value.
    // They only call Runtime::kAllocateHeapNumber.
    // The corresponding HChange instructions are added in a phase that does
    // not have easy access to the local context.
    __ mov(esi, Operand(ebp, StandardFrameConstants::kContextOffset));
    __ CallRuntimeSaveDoubles(Runtime::kAllocateHeapNumber);
    RecordSafepointWithRegisters(
        instr->pointer_map(), 0, Safepoint::kNoLazyDeopt);
    __ StoreToSafepointRegisterSlot(reg, eax);
  }

  // Done. Put the value in xmm_scratch into the value of the allocated heap
  // number.
  __ bind(&done);
  __ movsd(FieldOperand(reg, HeapNumber::kValueOffset), xmm_scratch);
}


void LCodeGen::DoNumberTagD(LNumberTagD* instr) {
  class DeferredNumberTagD final : public LDeferredCode {
   public:
    DeferredNumberTagD(LCodeGen* codegen, LNumberTagD* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override { codegen()->DoDeferredNumberTagD(instr_); }
    LInstruction* instr() override { return instr_; }

   private:
    LNumberTagD* instr_;
  };

  Register reg = ToRegister(instr->result());

  DeferredNumberTagD* deferred =
      new(zone()) DeferredNumberTagD(this, instr);
  if (FLAG_inline_new) {
    Register tmp = ToRegister(instr->temp());
    __ AllocateHeapNumber(reg, tmp, no_reg, deferred->entry());
  } else {
    __ jmp(deferred->entry());
  }
  __ bind(deferred->exit());
  XMMRegister input_reg = ToDoubleRegister(instr->value());
  __ movsd(FieldOperand(reg, HeapNumber::kValueOffset), input_reg);
}


void LCodeGen::DoDeferredNumberTagD(LNumberTagD* instr) {
  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  Register reg = ToRegister(instr->result());
  __ Move(reg, Immediate(0));

  PushSafepointRegistersScope scope(this);
  // NumberTagI and NumberTagD use the context from the frame, rather than
  // the environment's HContext or HInlinedContext value.
  // They only call Runtime::kAllocateHeapNumber.
  // The corresponding HChange instructions are added in a phase that does
  // not have easy access to the local context.
  __ mov(esi, Operand(ebp, StandardFrameConstants::kContextOffset));
  __ CallRuntimeSaveDoubles(Runtime::kAllocateHeapNumber);
  RecordSafepointWithRegisters(
      instr->pointer_map(), 0, Safepoint::kNoLazyDeopt);
  __ StoreToSafepointRegisterSlot(reg, eax);
}


void LCodeGen::DoSmiTag(LSmiTag* instr) {
  HChange* hchange = instr->hydrogen();
  Register input = ToRegister(instr->value());
  if (hchange->CheckFlag(HValue::kCanOverflow) &&
      hchange->value()->CheckFlag(HValue::kUint32)) {
    __ test(input, Immediate(0xc0000000));
    DeoptimizeIf(not_zero, instr, Deoptimizer::kOverflow);
  }
  __ SmiTag(input);
  if (hchange->CheckFlag(HValue::kCanOverflow) &&
      !hchange->value()->CheckFlag(HValue::kUint32)) {
    DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
  }
}


void LCodeGen::DoSmiUntag(LSmiUntag* instr) {
  LOperand* input = instr->value();
  Register result = ToRegister(input);
  DCHECK(input->IsRegister() && input->Equals(instr->result()));
  if (instr->needs_check()) {
    __ test(result, Immediate(kSmiTagMask));
    DeoptimizeIf(not_zero, instr, Deoptimizer::kNotASmi);
  } else {
    __ AssertSmi(result);
  }
  __ SmiUntag(result);
}


void LCodeGen::EmitNumberUntagD(LNumberUntagD* instr, Register input_reg,
                                Register temp_reg, XMMRegister result_reg,
                                NumberUntagDMode mode) {
  bool can_convert_undefined_to_nan =
      instr->hydrogen()->can_convert_undefined_to_nan();
  bool deoptimize_on_minus_zero = instr->hydrogen()->deoptimize_on_minus_zero();

  Label convert, load_smi, done;

  if (mode == NUMBER_CANDIDATE_IS_ANY_TAGGED) {
    // Smi check.
    __ JumpIfSmi(input_reg, &load_smi, Label::kNear);

    // Heap number map check.
    __ cmp(FieldOperand(input_reg, HeapObject::kMapOffset),
           factory()->heap_number_map());
    if (can_convert_undefined_to_nan) {
      __ j(not_equal, &convert, Label::kNear);
    } else {
      DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumber);
    }

    // Heap number to XMM conversion.
    __ movsd(result_reg, FieldOperand(input_reg, HeapNumber::kValueOffset));

    if (deoptimize_on_minus_zero) {
      XMMRegister xmm_scratch = double_scratch0();
      __ xorps(xmm_scratch, xmm_scratch);
      __ ucomisd(result_reg, xmm_scratch);
      __ j(not_zero, &done, Label::kNear);
      __ movmskpd(temp_reg, result_reg);
      __ test_b(temp_reg, 1);
      DeoptimizeIf(not_zero, instr, Deoptimizer::kMinusZero);
    }
    __ jmp(&done, Label::kNear);

    if (can_convert_undefined_to_nan) {
      __ bind(&convert);

      // Convert undefined to NaN.
      __ cmp(input_reg, factory()->undefined_value());
      DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumberUndefined);

      __ pcmpeqd(result_reg, result_reg);
      __ jmp(&done, Label::kNear);
    }
  } else {
    DCHECK(mode == NUMBER_CANDIDATE_IS_SMI);
  }

  __ bind(&load_smi);
  // Smi to XMM conversion. Clobbering a temp is faster than re-tagging the
  // input register since we avoid dependencies.
  __ mov(temp_reg, input_reg);
  __ SmiUntag(temp_reg);  // Untag smi before converting to float.
  __ Cvtsi2sd(result_reg, Operand(temp_reg));
  __ bind(&done);
}


void LCodeGen::DoDeferredTaggedToI(LTaggedToI* instr, Label* done) {
  Register input_reg = ToRegister(instr->value());

  // The input was optimistically untagged; revert it.
  STATIC_ASSERT(kSmiTagSize == 1);
  __ lea(input_reg, Operand(input_reg, times_2, kHeapObjectTag));

  if (instr->truncating()) {
    Label no_heap_number, check_bools, check_false;

    // Heap number map check.
    __ cmp(FieldOperand(input_reg, HeapObject::kMapOffset),
           factory()->heap_number_map());
    __ j(not_equal, &no_heap_number, Label::kNear);
    __ TruncateHeapNumberToI(input_reg, input_reg);
    __ jmp(done);

    __ bind(&no_heap_number);
    // Check for Oddballs. Undefined/False is converted to zero and True to one
    // for truncating conversions.
    __ cmp(input_reg, factory()->undefined_value());
    __ j(not_equal, &check_bools, Label::kNear);
    __ Move(input_reg, Immediate(0));
    __ jmp(done);

    __ bind(&check_bools);
    __ cmp(input_reg, factory()->true_value());
    __ j(not_equal, &check_false, Label::kNear);
    __ Move(input_reg, Immediate(1));
    __ jmp(done);

    __ bind(&check_false);
    __ cmp(input_reg, factory()->false_value());
    DeoptimizeIf(not_equal, instr,
                 Deoptimizer::kNotAHeapNumberUndefinedBoolean);
    __ Move(input_reg, Immediate(0));
  } else {
    XMMRegister scratch = ToDoubleRegister(instr->temp());
    DCHECK(!scratch.is(xmm0));
    __ cmp(FieldOperand(input_reg, HeapObject::kMapOffset),
           isolate()->factory()->heap_number_map());
    DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumber);
    __ movsd(xmm0, FieldOperand(input_reg, HeapNumber::kValueOffset));
    __ cvttsd2si(input_reg, Operand(xmm0));
    __ Cvtsi2sd(scratch, Operand(input_reg));
    __ ucomisd(xmm0, scratch);
    DeoptimizeIf(not_equal, instr, Deoptimizer::kLostPrecision);
    DeoptimizeIf(parity_even, instr, Deoptimizer::kNaN);
    if (instr->hydrogen()->GetMinusZeroMode() == FAIL_ON_MINUS_ZERO) {
      __ test(input_reg, Operand(input_reg));
      __ j(not_zero, done);
      __ movmskpd(input_reg, xmm0);
      __ and_(input_reg, 1);
      DeoptimizeIf(not_zero, instr, Deoptimizer::kMinusZero);
    }
  }
}


void LCodeGen::DoTaggedToI(LTaggedToI* instr) {
  class DeferredTaggedToI final : public LDeferredCode {
   public:
    DeferredTaggedToI(LCodeGen* codegen, LTaggedToI* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override { codegen()->DoDeferredTaggedToI(instr_, done()); }
    LInstruction* instr() override { return instr_; }

   private:
    LTaggedToI* instr_;
  };

  LOperand* input = instr->value();
  DCHECK(input->IsRegister());
  Register input_reg = ToRegister(input);
  DCHECK(input_reg.is(ToRegister(instr->result())));

  if (instr->hydrogen()->value()->representation().IsSmi()) {
    __ SmiUntag(input_reg);
  } else {
    DeferredTaggedToI* deferred =
        new(zone()) DeferredTaggedToI(this, instr);
    // Optimistically untag the input.
    // If the input is a HeapObject, SmiUntag will set the carry flag.
    STATIC_ASSERT(kSmiTagSize == 1 && kSmiTag == 0);
    __ SmiUntag(input_reg);
    // Branch to deferred code if the input was tagged.
    // The deferred code will take care of restoring the tag.
    __ j(carry, deferred->entry());
    __ bind(deferred->exit());
  }
}


void LCodeGen::DoNumberUntagD(LNumberUntagD* instr) {
  LOperand* input = instr->value();
  DCHECK(input->IsRegister());
  LOperand* temp = instr->temp();
  DCHECK(temp->IsRegister());
  LOperand* result = instr->result();
  DCHECK(result->IsDoubleRegister());

  Register input_reg = ToRegister(input);
  Register temp_reg = ToRegister(temp);

  HValue* value = instr->hydrogen()->value();
  NumberUntagDMode mode = value->representation().IsSmi()
      ? NUMBER_CANDIDATE_IS_SMI : NUMBER_CANDIDATE_IS_ANY_TAGGED;

  XMMRegister result_reg = ToDoubleRegister(result);
  EmitNumberUntagD(instr, input_reg, temp_reg, result_reg, mode);
}


void LCodeGen::DoDoubleToI(LDoubleToI* instr) {
  LOperand* input = instr->value();
  DCHECK(input->IsDoubleRegister());
  LOperand* result = instr->result();
  DCHECK(result->IsRegister());
  Register result_reg = ToRegister(result);

  if (instr->truncating()) {
    XMMRegister input_reg = ToDoubleRegister(input);
    __ TruncateDoubleToI(result_reg, input_reg);
  } else {
    Label lost_precision, is_nan, minus_zero, done;
    XMMRegister input_reg = ToDoubleRegister(input);
    XMMRegister xmm_scratch = double_scratch0();
    Label::Distance dist = DeoptEveryNTimes() ? Label::kFar : Label::kNear;
    __ DoubleToI(result_reg, input_reg, xmm_scratch,
                 instr->hydrogen()->GetMinusZeroMode(), &lost_precision,
                 &is_nan, &minus_zero, dist);
    __ jmp(&done, dist);
    __ bind(&lost_precision);
    DeoptimizeIf(no_condition, instr, Deoptimizer::kLostPrecision);
    __ bind(&is_nan);
    DeoptimizeIf(no_condition, instr, Deoptimizer::kNaN);
    __ bind(&minus_zero);
    DeoptimizeIf(no_condition, instr, Deoptimizer::kMinusZero);
    __ bind(&done);
  }
}


void LCodeGen::DoDoubleToSmi(LDoubleToSmi* instr) {
  LOperand* input = instr->value();
  DCHECK(input->IsDoubleRegister());
  LOperand* result = instr->result();
  DCHECK(result->IsRegister());
  Register result_reg = ToRegister(result);

  Label lost_precision, is_nan, minus_zero, done;
  XMMRegister input_reg = ToDoubleRegister(input);
  XMMRegister xmm_scratch = double_scratch0();
  Label::Distance dist = DeoptEveryNTimes() ? Label::kFar : Label::kNear;
  __ DoubleToI(result_reg, input_reg, xmm_scratch,
               instr->hydrogen()->GetMinusZeroMode(), &lost_precision, &is_nan,
               &minus_zero, dist);
  __ jmp(&done, dist);
  __ bind(&lost_precision);
  DeoptimizeIf(no_condition, instr, Deoptimizer::kLostPrecision);
  __ bind(&is_nan);
  DeoptimizeIf(no_condition, instr, Deoptimizer::kNaN);
  __ bind(&minus_zero);
  DeoptimizeIf(no_condition, instr, Deoptimizer::kMinusZero);
  __ bind(&done);
  __ SmiTag(result_reg);
  DeoptimizeIf(overflow, instr, Deoptimizer::kOverflow);
}


void LCodeGen::DoCheckSmi(LCheckSmi* instr) {
  LOperand* input = instr->value();
  __ test(ToOperand(input), Immediate(kSmiTagMask));
  DeoptimizeIf(not_zero, instr, Deoptimizer::kNotASmi);
}


void LCodeGen::DoCheckNonSmi(LCheckNonSmi* instr) {
  if (!instr->hydrogen()->value()->type().IsHeapObject()) {
    LOperand* input = instr->value();
    __ test(ToOperand(input), Immediate(kSmiTagMask));
    DeoptimizeIf(zero, instr, Deoptimizer::kSmi);
  }
}


void LCodeGen::DoCheckArrayBufferNotNeutered(
    LCheckArrayBufferNotNeutered* instr) {
  Register view = ToRegister(instr->view());
  Register scratch = ToRegister(instr->scratch());

  __ mov(scratch, FieldOperand(view, JSArrayBufferView::kBufferOffset));
  __ test_b(FieldOperand(scratch, JSArrayBuffer::kBitFieldOffset),
            1 << JSArrayBuffer::WasNeutered::kShift);
  DeoptimizeIf(not_zero, instr, Deoptimizer::kOutOfBounds);
}


void LCodeGen::DoCheckInstanceType(LCheckInstanceType* instr) {
  Register input = ToRegister(instr->value());
  Register temp = ToRegister(instr->temp());

  __ mov(temp, FieldOperand(input, HeapObject::kMapOffset));

  if (instr->hydrogen()->is_interval_check()) {
    InstanceType first;
    InstanceType last;
    instr->hydrogen()->GetCheckInterval(&first, &last);

    __ cmpb(FieldOperand(temp, Map::kInstanceTypeOffset),
            static_cast<int8_t>(first));

    // If there is only one type in the interval check for equality.
    if (first == last) {
      DeoptimizeIf(not_equal, instr, Deoptimizer::kWrongInstanceType);
    } else {
      DeoptimizeIf(below, instr, Deoptimizer::kWrongInstanceType);
      // Omit check for the last type.
      if (last != LAST_TYPE) {
        __ cmpb(FieldOperand(temp, Map::kInstanceTypeOffset),
                static_cast<int8_t>(last));
        DeoptimizeIf(above, instr, Deoptimizer::kWrongInstanceType);
      }
    }
  } else {
    uint8_t mask;
    uint8_t tag;
    instr->hydrogen()->GetCheckMaskAndTag(&mask, &tag);

    if (base::bits::IsPowerOfTwo32(mask)) {
      DCHECK(tag == 0 || base::bits::IsPowerOfTwo32(tag));
      __ test_b(FieldOperand(temp, Map::kInstanceTypeOffset), mask);
      DeoptimizeIf(tag == 0 ? not_zero : zero, instr,
                   Deoptimizer::kWrongInstanceType);
    } else {
      __ movzx_b(temp, FieldOperand(temp, Map::kInstanceTypeOffset));
      __ and_(temp, mask);
      __ cmp(temp, tag);
      DeoptimizeIf(not_equal, instr, Deoptimizer::kWrongInstanceType);
    }
  }
}


void LCodeGen::DoCheckValue(LCheckValue* instr) {
  Handle<HeapObject> object = instr->hydrogen()->object().handle();
  if (instr->hydrogen()->object_in_new_space()) {
    Register reg = ToRegister(instr->value());
    Handle<Cell> cell = isolate()->factory()->NewCell(object);
    __ cmp(reg, Operand::ForCell(cell));
  } else {
    Operand operand = ToOperand(instr->value());
    __ cmp(operand, object);
  }
  DeoptimizeIf(not_equal, instr, Deoptimizer::kValueMismatch);
}


void LCodeGen::DoDeferredInstanceMigration(LCheckMaps* instr, Register object) {
  {
    PushSafepointRegistersScope scope(this);
    __ push(object);
    __ xor_(esi, esi);
    __ CallRuntimeSaveDoubles(Runtime::kTryMigrateInstance);
    RecordSafepointWithRegisters(
        instr->pointer_map(), 1, Safepoint::kNoLazyDeopt);

    __ test(eax, Immediate(kSmiTagMask));
  }
  DeoptimizeIf(zero, instr, Deoptimizer::kInstanceMigrationFailed);
}


void LCodeGen::DoCheckMaps(LCheckMaps* instr) {
  class DeferredCheckMaps final : public LDeferredCode {
   public:
    DeferredCheckMaps(LCodeGen* codegen, LCheckMaps* instr,  Register object)
        : LDeferredCode(codegen), instr_(instr), object_(object) {
      SetExit(check_maps());
    }
    void Generate() override {
      codegen()->DoDeferredInstanceMigration(instr_, object_);
    }
    Label* check_maps() { return &check_maps_; }
    LInstruction* instr() override { return instr_; }

   private:
    LCheckMaps* instr_;
    Label check_maps_;
    Register object_;
  };

  if (instr->hydrogen()->IsStabilityCheck()) {
    const UniqueSet<Map>* maps = instr->hydrogen()->maps();
    for (int i = 0; i < maps->size(); ++i) {
      AddStabilityDependency(maps->at(i).handle());
    }
    return;
  }

  LOperand* input = instr->value();
  DCHECK(input->IsRegister());
  Register reg = ToRegister(input);

  DeferredCheckMaps* deferred = NULL;
  if (instr->hydrogen()->HasMigrationTarget()) {
    deferred = new(zone()) DeferredCheckMaps(this, instr, reg);
    __ bind(deferred->check_maps());
  }

  const UniqueSet<Map>* maps = instr->hydrogen()->maps();
  Label success;
  for (int i = 0; i < maps->size() - 1; i++) {
    Handle<Map> map = maps->at(i).handle();
    __ CompareMap(reg, map);
    __ j(equal, &success, Label::kNear);
  }

  Handle<Map> map = maps->at(maps->size() - 1).handle();
  __ CompareMap(reg, map);
  if (instr->hydrogen()->HasMigrationTarget()) {
    __ j(not_equal, deferred->entry());
  } else {
    DeoptimizeIf(not_equal, instr, Deoptimizer::kWrongMap);
  }

  __ bind(&success);
}


void LCodeGen::DoClampDToUint8(LClampDToUint8* instr) {
  XMMRegister value_reg = ToDoubleRegister(instr->unclamped());
  XMMRegister xmm_scratch = double_scratch0();
  Register result_reg = ToRegister(instr->result());
  __ ClampDoubleToUint8(value_reg, xmm_scratch, result_reg);
}


void LCodeGen::DoClampIToUint8(LClampIToUint8* instr) {
  DCHECK(instr->unclamped()->Equals(instr->result()));
  Register value_reg = ToRegister(instr->result());
  __ ClampUint8(value_reg);
}


void LCodeGen::DoClampTToUint8(LClampTToUint8* instr) {
  DCHECK(instr->unclamped()->Equals(instr->result()));
  Register input_reg = ToRegister(instr->unclamped());
  XMMRegister temp_xmm_reg = ToDoubleRegister(instr->temp_xmm());
  XMMRegister xmm_scratch = double_scratch0();
  Label is_smi, done, heap_number;

  __ JumpIfSmi(input_reg, &is_smi);

  // Check for heap number
  __ cmp(FieldOperand(input_reg, HeapObject::kMapOffset),
         factory()->heap_number_map());
  __ j(equal, &heap_number, Label::kNear);

  // Check for undefined. Undefined is converted to zero for clamping
  // conversions.
  __ cmp(input_reg, factory()->undefined_value());
  DeoptimizeIf(not_equal, instr, Deoptimizer::kNotAHeapNumberUndefined);
  __ mov(input_reg, 0);
  __ jmp(&done, Label::kNear);

  // Heap number
  __ bind(&heap_number);
  __ movsd(xmm_scratch, FieldOperand(input_reg, HeapNumber::kValueOffset));
  __ ClampDoubleToUint8(xmm_scratch, temp_xmm_reg, input_reg);
  __ jmp(&done, Label::kNear);

  // smi
  __ bind(&is_smi);
  __ SmiUntag(input_reg);
  __ ClampUint8(input_reg);
  __ bind(&done);
}


void LCodeGen::DoDoubleBits(LDoubleBits* instr) {
  XMMRegister value_reg = ToDoubleRegister(instr->value());
  Register result_reg = ToRegister(instr->result());
  if (instr->hydrogen()->bits() == HDoubleBits::HIGH) {
    if (CpuFeatures::IsSupported(SSE4_1)) {
      CpuFeatureScope scope2(masm(), SSE4_1);
      __ pextrd(result_reg, value_reg, 1);
    } else {
      XMMRegister xmm_scratch = double_scratch0();
      __ pshufd(xmm_scratch, value_reg, 1);
      __ movd(result_reg, xmm_scratch);
    }
  } else {
    __ movd(result_reg, value_reg);
  }
}


void LCodeGen::DoConstructDouble(LConstructDouble* instr) {
  Register hi_reg = ToRegister(instr->hi());
  Register lo_reg = ToRegister(instr->lo());
  XMMRegister result_reg = ToDoubleRegister(instr->result());

  if (CpuFeatures::IsSupported(SSE4_1)) {
    CpuFeatureScope scope2(masm(), SSE4_1);
    __ movd(result_reg, lo_reg);
    __ pinsrd(result_reg, hi_reg, 1);
  } else {
    XMMRegister xmm_scratch = double_scratch0();
    __ movd(result_reg, hi_reg);
    __ psllq(result_reg, 32);
    __ movd(xmm_scratch, lo_reg);
    __ orps(result_reg, xmm_scratch);
  }
}


void LCodeGen::DoAllocate(LAllocate* instr) {
  class DeferredAllocate final : public LDeferredCode {
   public:
    DeferredAllocate(LCodeGen* codegen,  LAllocate* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override { codegen()->DoDeferredAllocate(instr_); }
    LInstruction* instr() override { return instr_; }

   private:
    LAllocate* instr_;
  };

  DeferredAllocate* deferred = new(zone()) DeferredAllocate(this, instr);

  Register result = ToRegister(instr->result());
  Register temp = ToRegister(instr->temp());

  // Allocate memory for the object.
  AllocationFlags flags = TAG_OBJECT;
  if (instr->hydrogen()->MustAllocateDoubleAligned()) {
    flags = static_cast<AllocationFlags>(flags | DOUBLE_ALIGNMENT);
  }
  if (instr->hydrogen()->IsOldSpaceAllocation()) {
    DCHECK(!instr->hydrogen()->IsNewSpaceAllocation());
    flags = static_cast<AllocationFlags>(flags | PRETENURE);
  }

  if (instr->size()->IsConstantOperand()) {
    int32_t size = ToInteger32(LConstantOperand::cast(instr->size()));
    if (size <= Page::kMaxRegularHeapObjectSize) {
      __ Allocate(size, result, temp, no_reg, deferred->entry(), flags);
    } else {
      __ jmp(deferred->entry());
    }
  } else {
    Register size = ToRegister(instr->size());
    __ Allocate(size, result, temp, no_reg, deferred->entry(), flags);
  }

  __ bind(deferred->exit());

  if (instr->hydrogen()->MustPrefillWithFiller()) {
    if (instr->size()->IsConstantOperand()) {
      int32_t size = ToInteger32(LConstantOperand::cast(instr->size()));
      __ mov(temp, (size / kPointerSize) - 1);
    } else {
      temp = ToRegister(instr->size());
      __ shr(temp, kPointerSizeLog2);
      __ dec(temp);
    }
    Label loop;
    __ bind(&loop);
    __ mov(FieldOperand(result, temp, times_pointer_size, 0),
        isolate()->factory()->one_pointer_filler_map());
    __ dec(temp);
    __ j(not_zero, &loop);
  }
}


void LCodeGen::DoDeferredAllocate(LAllocate* instr) {
  Register result = ToRegister(instr->result());

  // TODO(3095996): Get rid of this. For now, we need to make the
  // result register contain a valid pointer because it is already
  // contained in the register pointer map.
  __ Move(result, Immediate(Smi::FromInt(0)));

  PushSafepointRegistersScope scope(this);
  if (instr->size()->IsRegister()) {
    Register size = ToRegister(instr->size());
    DCHECK(!size.is(result));
    __ SmiTag(ToRegister(instr->size()));
    __ push(size);
  } else {
    int32_t size = ToInteger32(LConstantOperand::cast(instr->size()));
    if (size >= 0 && size <= Smi::kMaxValue) {
      __ push(Immediate(Smi::FromInt(size)));
    } else {
      // We should never get here at runtime => abort
      __ int3();
      return;
    }
  }

  int flags = AllocateDoubleAlignFlag::encode(
      instr->hydrogen()->MustAllocateDoubleAligned());
  if (instr->hydrogen()->IsOldSpaceAllocation()) {
    DCHECK(!instr->hydrogen()->IsNewSpaceAllocation());
    flags = AllocateTargetSpace::update(flags, OLD_SPACE);
  } else {
    flags = AllocateTargetSpace::update(flags, NEW_SPACE);
  }
  __ push(Immediate(Smi::FromInt(flags)));

  CallRuntimeFromDeferred(
      Runtime::kAllocateInTargetSpace, 2, instr, instr->context());
  __ StoreToSafepointRegisterSlot(result, eax);
}


void LCodeGen::DoToFastProperties(LToFastProperties* instr) {
  DCHECK(ToRegister(instr->value()).is(eax));
  __ push(eax);
  CallRuntime(Runtime::kToFastProperties, 1, instr);
}


void LCodeGen::DoRegExpLiteral(LRegExpLiteral* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  Label materialized;
  // Registers will be used as follows:
  // ecx = literals array.
  // ebx = regexp literal.
  // eax = regexp literal clone.
  // esi = context.
  int literal_offset =
      FixedArray::OffsetOfElementAt(instr->hydrogen()->literal_index());
  __ LoadHeapObject(ecx, instr->hydrogen()->literals());
  __ mov(ebx, FieldOperand(ecx, literal_offset));
  __ cmp(ebx, factory()->undefined_value());
  __ j(not_equal, &materialized, Label::kNear);

  // Create regexp literal using runtime function
  // Result will be in eax.
  __ push(ecx);
  __ push(Immediate(Smi::FromInt(instr->hydrogen()->literal_index())));
  __ push(Immediate(instr->hydrogen()->pattern()));
  __ push(Immediate(instr->hydrogen()->flags()));
  CallRuntime(Runtime::kMaterializeRegExpLiteral, 4, instr);
  __ mov(ebx, eax);

  __ bind(&materialized);
  int size = JSRegExp::kSize + JSRegExp::kInObjectFieldCount * kPointerSize;
  Label allocated, runtime_allocate;
  __ Allocate(size, eax, ecx, edx, &runtime_allocate, TAG_OBJECT);
  __ jmp(&allocated, Label::kNear);

  __ bind(&runtime_allocate);
  __ push(ebx);
  __ push(Immediate(Smi::FromInt(size)));
  CallRuntime(Runtime::kAllocateInNewSpace, 1, instr);
  __ pop(ebx);

  __ bind(&allocated);
  // Copy the content into the newly allocated memory.
  // (Unroll copy loop once for better throughput).
  for (int i = 0; i < size - kPointerSize; i += 2 * kPointerSize) {
    __ mov(edx, FieldOperand(ebx, i));
    __ mov(ecx, FieldOperand(ebx, i + kPointerSize));
    __ mov(FieldOperand(eax, i), edx);
    __ mov(FieldOperand(eax, i + kPointerSize), ecx);
  }
  if ((size % (2 * kPointerSize)) != 0) {
    __ mov(edx, FieldOperand(ebx, size - kPointerSize));
    __ mov(FieldOperand(eax, size - kPointerSize), edx);
  }
}


void LCodeGen::DoFunctionLiteral(LFunctionLiteral* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  // Use the fast case closure allocation code that allocates in new
  // space for nested functions that don't need literals cloning.
  bool pretenure = instr->hydrogen()->pretenure();
  if (!pretenure && instr->hydrogen()->has_no_literals()) {
    FastNewClosureStub stub(isolate(), instr->hydrogen()->language_mode(),
                            instr->hydrogen()->kind());
    __ mov(ebx, Immediate(instr->hydrogen()->shared_info()));
    CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
  } else {
    __ push(esi);
    __ push(Immediate(instr->hydrogen()->shared_info()));
    __ push(Immediate(pretenure ? factory()->true_value()
                                : factory()->false_value()));
    CallRuntime(Runtime::kNewClosure, 3, instr);
  }
}


void LCodeGen::DoTypeof(LTypeof* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  DCHECK(ToRegister(instr->value()).is(ebx));
  Label end, do_call;
  Register value_register = ToRegister(instr->value());
  __ JumpIfNotSmi(value_register, &do_call);
  __ mov(eax, Immediate(isolate()->factory()->number_string()));
  __ jmp(&end);
  __ bind(&do_call);
  TypeofStub stub(isolate());
  CallCode(stub.GetCode(), RelocInfo::CODE_TARGET, instr);
  __ bind(&end);
}


void LCodeGen::DoTypeofIsAndBranch(LTypeofIsAndBranch* instr) {
  Register input = ToRegister(instr->value());
  Condition final_branch_condition = EmitTypeofIs(instr, input);
  if (final_branch_condition != no_condition) {
    EmitBranch(instr, final_branch_condition);
  }
}


Condition LCodeGen::EmitTypeofIs(LTypeofIsAndBranch* instr, Register input) {
  Label* true_label = instr->TrueLabel(chunk_);
  Label* false_label = instr->FalseLabel(chunk_);
  Handle<String> type_name = instr->type_literal();
  int left_block = instr->TrueDestination(chunk_);
  int right_block = instr->FalseDestination(chunk_);
  int next_block = GetNextEmittedBlock();

  Label::Distance true_distance = left_block == next_block ? Label::kNear
                                                           : Label::kFar;
  Label::Distance false_distance = right_block == next_block ? Label::kNear
                                                             : Label::kFar;
  Condition final_branch_condition = no_condition;
  if (String::Equals(type_name, factory()->number_string())) {
    __ JumpIfSmi(input, true_label, true_distance);
    __ cmp(FieldOperand(input, HeapObject::kMapOffset),
           factory()->heap_number_map());
    final_branch_condition = equal;

  } else if (String::Equals(type_name, factory()->string_string())) {
    __ JumpIfSmi(input, false_label, false_distance);
    __ CmpObjectType(input, FIRST_NONSTRING_TYPE, input);
    __ j(above_equal, false_label, false_distance);
    __ test_b(FieldOperand(input, Map::kBitFieldOffset),
              1 << Map::kIsUndetectable);
    final_branch_condition = zero;

  } else if (String::Equals(type_name, factory()->symbol_string())) {
    __ JumpIfSmi(input, false_label, false_distance);
    __ CmpObjectType(input, SYMBOL_TYPE, input);
    final_branch_condition = equal;

  } else if (String::Equals(type_name, factory()->boolean_string())) {
    __ cmp(input, factory()->true_value());
    __ j(equal, true_label, true_distance);
    __ cmp(input, factory()->false_value());
    final_branch_condition = equal;

  } else if (String::Equals(type_name, factory()->undefined_string())) {
    __ cmp(input, factory()->undefined_value());
    __ j(equal, true_label, true_distance);
    __ JumpIfSmi(input, false_label, false_distance);
    // Check for undetectable objects => true.
    __ mov(input, FieldOperand(input, HeapObject::kMapOffset));
    __ test_b(FieldOperand(input, Map::kBitFieldOffset),
              1 << Map::kIsUndetectable);
    final_branch_condition = not_zero;

  } else if (String::Equals(type_name, factory()->function_string())) {
    STATIC_ASSERT(NUM_OF_CALLABLE_SPEC_OBJECT_TYPES == 2);
    __ JumpIfSmi(input, false_label, false_distance);
    __ CmpObjectType(input, JS_FUNCTION_TYPE, input);
    __ j(equal, true_label, true_distance);
    __ CmpInstanceType(input, JS_FUNCTION_PROXY_TYPE);
    final_branch_condition = equal;

  } else if (String::Equals(type_name, factory()->object_string())) {
    __ JumpIfSmi(input, false_label, false_distance);
    __ cmp(input, factory()->null_value());
    __ j(equal, true_label, true_distance);
    __ CmpObjectType(input, FIRST_NONCALLABLE_SPEC_OBJECT_TYPE, input);
    __ j(below, false_label, false_distance);
    __ CmpInstanceType(input, LAST_NONCALLABLE_SPEC_OBJECT_TYPE);
    __ j(above, false_label, false_distance);
    // Check for undetectable objects => false.
    __ test_b(FieldOperand(input, Map::kBitFieldOffset),
              1 << Map::kIsUndetectable);
    final_branch_condition = zero;

  } else {
    __ jmp(false_label, false_distance);
  }
  return final_branch_condition;
}


void LCodeGen::DoIsConstructCallAndBranch(LIsConstructCallAndBranch* instr) {
  Register temp = ToRegister(instr->temp());

  EmitIsConstructCall(temp);
  EmitBranch(instr, equal);
}


void LCodeGen::EmitIsConstructCall(Register temp) {
  // Get the frame pointer for the calling frame.
  __ mov(temp, Operand(ebp, StandardFrameConstants::kCallerFPOffset));

  // Skip the arguments adaptor frame if it exists.
  Label check_frame_marker;
  __ cmp(Operand(temp, StandardFrameConstants::kContextOffset),
         Immediate(Smi::FromInt(StackFrame::ARGUMENTS_ADAPTOR)));
  __ j(not_equal, &check_frame_marker, Label::kNear);
  __ mov(temp, Operand(temp, StandardFrameConstants::kCallerFPOffset));

  // Check the marker in the calling frame.
  __ bind(&check_frame_marker);
  __ cmp(Operand(temp, StandardFrameConstants::kMarkerOffset),
         Immediate(Smi::FromInt(StackFrame::CONSTRUCT)));
}


void LCodeGen::EnsureSpaceForLazyDeopt(int space_needed) {
  if (!info()->IsStub()) {
    // Ensure that we have enough space after the previous lazy-bailout
    // instruction for patching the code here.
    int current_pc = masm()->pc_offset();
    if (current_pc < last_lazy_deopt_pc_ + space_needed) {
      int padding_size = last_lazy_deopt_pc_ + space_needed - current_pc;
      __ Nop(padding_size);
    }
  }
  last_lazy_deopt_pc_ = masm()->pc_offset();
}


void LCodeGen::DoLazyBailout(LLazyBailout* instr) {
  last_lazy_deopt_pc_ = masm()->pc_offset();
  DCHECK(instr->HasEnvironment());
  LEnvironment* env = instr->environment();
  RegisterEnvironmentForDeoptimization(env, Safepoint::kLazyDeopt);
  safepoints_.RecordLazyDeoptimizationIndex(env->deoptimization_index());
}


void LCodeGen::DoDeoptimize(LDeoptimize* instr) {
  Deoptimizer::BailoutType type = instr->hydrogen()->type();
  // TODO(danno): Stubs expect all deopts to be lazy for historical reasons (the
  // needed return address), even though the implementation of LAZY and EAGER is
  // now identical. When LAZY is eventually completely folded into EAGER, remove
  // the special case below.
  if (info()->IsStub() && type == Deoptimizer::EAGER) {
    type = Deoptimizer::LAZY;
  }
  DeoptimizeIf(no_condition, instr, instr->hydrogen()->reason(), type);
}


void LCodeGen::DoDummy(LDummy* instr) {
  // Nothing to see here, move on!
}


void LCodeGen::DoDummyUse(LDummyUse* instr) {
  // Nothing to see here, move on!
}


void LCodeGen::DoDeferredStackCheck(LStackCheck* instr) {
  PushSafepointRegistersScope scope(this);
  __ mov(esi, Operand(ebp, StandardFrameConstants::kContextOffset));
  __ CallRuntimeSaveDoubles(Runtime::kStackGuard);
  RecordSafepointWithLazyDeopt(
      instr, RECORD_SAFEPOINT_WITH_REGISTERS_AND_NO_ARGUMENTS);
  DCHECK(instr->HasEnvironment());
  LEnvironment* env = instr->environment();
  safepoints_.RecordLazyDeoptimizationIndex(env->deoptimization_index());
}


void LCodeGen::DoStackCheck(LStackCheck* instr) {
  class DeferredStackCheck final : public LDeferredCode {
   public:
    DeferredStackCheck(LCodeGen* codegen, LStackCheck* instr)
        : LDeferredCode(codegen), instr_(instr) { }
    void Generate() override { codegen()->DoDeferredStackCheck(instr_); }
    LInstruction* instr() override { return instr_; }

   private:
    LStackCheck* instr_;
  };

  DCHECK(instr->HasEnvironment());
  LEnvironment* env = instr->environment();
  // There is no LLazyBailout instruction for stack-checks. We have to
  // prepare for lazy deoptimization explicitly here.
  if (instr->hydrogen()->is_function_entry()) {
    // Perform stack overflow check.
    Label done;
    ExternalReference stack_limit =
        ExternalReference::address_of_stack_limit(isolate());
    __ cmp(esp, Operand::StaticVariable(stack_limit));
    __ j(above_equal, &done, Label::kNear);

    DCHECK(instr->context()->IsRegister());
    DCHECK(ToRegister(instr->context()).is(esi));
    CallCode(isolate()->builtins()->StackCheck(),
             RelocInfo::CODE_TARGET,
             instr);
    __ bind(&done);
  } else {
    DCHECK(instr->hydrogen()->is_backwards_branch());
    // Perform stack overflow check if this goto needs it before jumping.
    DeferredStackCheck* deferred_stack_check =
        new(zone()) DeferredStackCheck(this, instr);
    ExternalReference stack_limit =
        ExternalReference::address_of_stack_limit(isolate());
    __ cmp(esp, Operand::StaticVariable(stack_limit));
    __ j(below, deferred_stack_check->entry());
    EnsureSpaceForLazyDeopt(Deoptimizer::patch_size());
    __ bind(instr->done_label());
    deferred_stack_check->SetExit(instr->done_label());
    RegisterEnvironmentForDeoptimization(env, Safepoint::kLazyDeopt);
    // Don't record a deoptimization index for the safepoint here.
    // This will be done explicitly when emitting call and the safepoint in
    // the deferred code.
  }
}


void LCodeGen::DoOsrEntry(LOsrEntry* instr) {
  // This is a pseudo-instruction that ensures that the environment here is
  // properly registered for deoptimization and records the assembler's PC
  // offset.
  LEnvironment* environment = instr->environment();

  // If the environment were already registered, we would have no way of
  // backpatching it with the spill slot operands.
  DCHECK(!environment->HasBeenRegistered());
  RegisterEnvironmentForDeoptimization(environment, Safepoint::kNoLazyDeopt);

  GenerateOsrPrologue();
}


void LCodeGen::DoForInPrepareMap(LForInPrepareMap* instr) {
  DCHECK(ToRegister(instr->context()).is(esi));
  __ test(eax, Immediate(kSmiTagMask));
  DeoptimizeIf(zero, instr, Deoptimizer::kSmi);

  STATIC_ASSERT(FIRST_JS_PROXY_TYPE == FIRST_SPEC_OBJECT_TYPE);
  __ CmpObjectType(eax, LAST_JS_PROXY_TYPE, ecx);
  DeoptimizeIf(below_equal, instr, Deoptimizer::kWrongInstanceType);

  Label use_cache, call_runtime;
  __ CheckEnumCache(&call_runtime);

  __ mov(eax, FieldOperand(eax, HeapObject::kMapOffset));
  __ jmp(&use_cache, Label::kNear);

  // Get the set of properties to enumerate.
  __ bind(&call_runtime);
  __ push(eax);
  CallRuntime(Runtime::kGetPropertyNamesFast, 1, instr);

  __ cmp(FieldOperand(eax, HeapObject::kMapOffset),
         isolate()->factory()->meta_map());
  DeoptimizeIf(not_equal, instr, Deoptimizer::kWrongMap);
  __ bind(&use_cache);
}


void LCodeGen::DoForInCacheArray(LForInCacheArray* instr) {
  Register map = ToRegister(instr->map());
  Register result = ToRegister(instr->result());
  Label load_cache, done;
  __ EnumLength(result, map);
  __ cmp(result, Immediate(Smi::FromInt(0)));
  __ j(not_equal, &load_cache, Label::kNear);
  __ mov(result, isolate()->factory()->empty_fixed_array());
  __ jmp(&done, Label::kNear);

  __ bind(&load_cache);
  __ LoadInstanceDescriptors(map, result);
  __ mov(result,
         FieldOperand(result, DescriptorArray::kEnumCacheOffset));
  __ mov(result,
         FieldOperand(result, FixedArray::SizeFor(instr->idx())));
  __ bind(&done);
  __ test(result, result);
  DeoptimizeIf(equal, instr, Deoptimizer::kNoCache);
}


void LCodeGen::DoCheckMapValue(LCheckMapValue* instr) {
  Register object = ToRegister(instr->value());
  __ cmp(ToRegister(instr->map()),
         FieldOperand(object, HeapObject::kMapOffset));
  DeoptimizeIf(not_equal, instr, Deoptimizer::kWrongMap);
}


void LCodeGen::DoDeferredLoadMutableDouble(LLoadFieldByIndex* instr,
                                           Register object,
                                           Register index) {
  PushSafepointRegistersScope scope(this);
  __ push(object);
  __ push(index);
  __ xor_(esi, esi);
  __ CallRuntimeSaveDoubles(Runtime::kLoadMutableDouble);
  RecordSafepointWithRegisters(
      instr->pointer_map(), 2, Safepoint::kNoLazyDeopt);
  __ StoreToSafepointRegisterSlot(object, eax);
}


void LCodeGen::DoLoadFieldByIndex(LLoadFieldByIndex* instr) {
  class DeferredLoadMutableDouble final : public LDeferredCode {
   public:
    DeferredLoadMutableDouble(LCodeGen* codegen,
                              LLoadFieldByIndex* instr,
                              Register object,
                              Register index)
        : LDeferredCode(codegen),
          instr_(instr),
          object_(object),
          index_(index) {
    }
    void Generate() override {
      codegen()->DoDeferredLoadMutableDouble(instr_, object_, index_);
    }
    LInstruction* instr() override { return instr_; }

   private:
    LLoadFieldByIndex* instr_;
    Register object_;
    Register index_;
  };

  Register object = ToRegister(instr->object());
  Register index = ToRegister(instr->index());

  DeferredLoadMutableDouble* deferred;
  deferred = new(zone()) DeferredLoadMutableDouble(
      this, instr, object, index);

  Label out_of_object, done;
  __ test(index, Immediate(Smi::FromInt(1)));
  __ j(not_zero, deferred->entry());

  __ sar(index, 1);

  __ cmp(index, Immediate(0));
  __ j(less, &out_of_object, Label::kNear);
  __ mov(object, FieldOperand(object,
                              index,
                              times_half_pointer_size,
                              JSObject::kHeaderSize));
  __ jmp(&done, Label::kNear);

  __ bind(&out_of_object);
  __ mov(object, FieldOperand(object, JSObject::kPropertiesOffset));
  __ neg(index);
  // Index is now equal to out of object property index plus 1.
  __ mov(object, FieldOperand(object,
                              index,
                              times_half_pointer_size,
                              FixedArray::kHeaderSize - kPointerSize));
  __ bind(deferred->exit());
  __ bind(&done);
}


void LCodeGen::DoStoreFrameContext(LStoreFrameContext* instr) {
  Register context = ToRegister(instr->context());
  __ mov(Operand(ebp, StandardFrameConstants::kContextOffset), context);
}


void LCodeGen::DoAllocateBlockContext(LAllocateBlockContext* instr) {
  Handle<ScopeInfo> scope_info = instr->scope_info();
  __ Push(scope_info);
  __ push(ToRegister(instr->function()));
  CallRuntime(Runtime::kPushBlockContext, 2, instr);
  RecordSafepoint(Safepoint::kNoLazyDeopt);
}


template<class T>
void LCodeGen::HandleSIMD128ToTagged(LSIMD128ToTagged* instr) {
  class DeferredSIMD128ToTagged final : public LDeferredCode {
   public:
    DeferredSIMD128ToTagged(LCodeGen* codegen,
        LInstruction* instr,
        Runtime::FunctionId id)
      : LDeferredCode(codegen), instr_(instr), id_(id) { }
    virtual void Generate() override {
      codegen()->DoDeferredSIMD128ToTagged(instr_, id_);
    }
    virtual LInstruction* instr() override { return instr_; }
   private:
    LInstruction* instr_;
    Runtime::FunctionId id_;
  };

  XMMRegister input_reg = ToSIMD128Register(instr->value());
  Register reg = ToRegister(instr->result());
  Register tmp = ToRegister(instr->temp());
  Register tmp2 = ToRegister(instr->temp2());

  DeferredSIMD128ToTagged* deferred = new(zone()) DeferredSIMD128ToTagged(
      this, instr, static_cast<Runtime::FunctionId>(T::kRuntimeAllocatorId()));

  if (FLAG_inline_new) {
    if (T::kInstanceType == FLOAT32x4_TYPE) {
      __ AllocateFloat32x4(reg, tmp, tmp2, deferred->entry());
    } else if (T::kInstanceType == INT32x4_TYPE) {
      __ AllocateInt32x4(reg, tmp, tmp2, deferred->entry());
    } else if (T::kInstanceType == FLOAT64x2_TYPE) {
      __ AllocateFloat64x2(reg, tmp, tmp2, deferred->entry());
    }
  } else {
    __ jmp(deferred->entry());
  }
  __ bind(deferred->exit());

  // Load the inner FixedTypedArray object.
  __ mov(tmp, FieldOperand(reg, T::kValueOffset));

  __ movups(FieldOperand(tmp, FixedTypedArrayBase::kDataOffset), input_reg);
}


void LCodeGen::DoSIMD128ToTagged(LSIMD128ToTagged* instr) {
  if (instr->value()->IsFloat32x4Register()) {
    HandleSIMD128ToTagged<Float32x4>(instr);
  } else if (instr->value()->IsFloat64x2Register()) {
    HandleSIMD128ToTagged<Float64x2>(instr);
  } else {
    DCHECK(instr->value()->IsInt32x4Register());
    HandleSIMD128ToTagged<Int32x4>(instr);
  }
}


template<class T>
void LCodeGen::HandleTaggedToSIMD128(LTaggedToSIMD128* instr) {
  LOperand* input = instr->value();
  DCHECK(input->IsRegister());
  LOperand* result = instr->result();
  DCHECK(result->IsSIMD128Register());

  Register input_reg = ToRegister(input);
  Register temp_reg = ToRegister(instr->temp());
  XMMRegister result_reg = ToSIMD128Register(result);

  __ test(input_reg, Immediate(kSmiTagMask));
  DeoptimizeIf(zero, instr, Deoptimizer::kSmi);
  __ CmpObjectType(input_reg, T::kInstanceType, temp_reg);
  DeoptimizeIf(not_equal, instr, Deoptimizer::kNotASIMD128);

  // Load the inner FixedTypedArray object.
  __ mov(temp_reg, FieldOperand(input_reg, T::kValueOffset));

  __ movups(
      result_reg, FieldOperand(temp_reg, FixedTypedArrayBase::kDataOffset));
}


void LCodeGen::DoTaggedToSIMD128(LTaggedToSIMD128* instr) {
  if (instr->representation().IsFloat32x4()) {
    HandleTaggedToSIMD128<Float32x4>(instr);
  } else if (instr->representation().IsFloat64x2()) {
    HandleTaggedToSIMD128<Float64x2>(instr);
  } else {
    DCHECK(instr->representation().IsInt32x4());
    HandleTaggedToSIMD128<Int32x4>(instr);
  }
}


void LCodeGen::DoNullarySIMDOperation(LNullarySIMDOperation* instr) {
  switch (instr->op()) {
    case kFloat32x4Zero: {
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      __ xorps(result_reg, result_reg);
      return;
    }
    case kFloat64x2Zero: {
      XMMRegister result_reg = ToFloat64x2Register(instr->result());
      __ xorpd(result_reg, result_reg);
      return;
    }
    case kInt32x4Zero: {
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      __ xorps(result_reg, result_reg);
      return;
    }
    default:
      UNREACHABLE();
      return;
  }
}


void LCodeGen::DoUnarySIMDOperation(LUnarySIMDOperation* instr) {
  uint8_t select = 0;
  switch (instr->op()) {
    case kFloat32x4Check: {
      XMMRegister input_reg = ToFloat32x4Register(instr->value());
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      if (!result_reg.is(input_reg)) {
        __ movaps(result_reg, input_reg);
      }
      return;
    }
    case kFloat64x2Check: {
      XMMRegister input_reg = ToFloat64x2Register(instr->value());
      XMMRegister result_reg = ToFloat64x2Register(instr->result());
      if (!result_reg.is(input_reg)) {
        __ movaps(result_reg, input_reg);
      }
      return;
    }
    case kInt32x4Check: {
      XMMRegister input_reg = ToInt32x4Register(instr->value());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      if (!result_reg.is(input_reg)) {
        __ movaps(result_reg, input_reg);
      }
      return;
    }
    case kSIMD128Change: {
      Comment(";;; deoptimize: can not perform representation change"
              "for float32x4 or int32x4");
      DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
      return;
    }
    case kFloat32x4Abs:
    case kFloat32x4Neg:
    case kFloat32x4Reciprocal:
    case kFloat32x4ReciprocalSqrt:
    case kFloat32x4Sqrt: {
      DCHECK(instr->value()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->value()->representation().IsFloat32x4());
      XMMRegister input_reg = ToFloat32x4Register(instr->value());
      switch (instr->op()) {
        case kFloat32x4Abs:
          __ absps(input_reg);
          break;
        case kFloat32x4Neg:
          __ negateps(input_reg);
          break;
        case kFloat32x4Reciprocal:
          __ rcpps(input_reg, input_reg);
          break;
        case kFloat32x4ReciprocalSqrt:
          __ rsqrtps(input_reg, input_reg);
          break;
        case kFloat32x4Sqrt:
          __ sqrtps(input_reg, input_reg);
          break;
        default:
          UNREACHABLE();
          break;
        }
      return;
    }
    case kFloat64x2Abs:
    case kFloat64x2Neg:
    case kFloat64x2Sqrt: {
      DCHECK(instr->value()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->value()->representation().IsFloat64x2());
      XMMRegister input_reg = ToFloat64x2Register(instr->value());
      switch (instr->op()) {
        case kFloat64x2Abs:
          __ abspd(input_reg);
          break;
        case kFloat64x2Neg:
          __ negatepd(input_reg);
          break;
        case kFloat64x2Sqrt:
          __ sqrtpd(input_reg, input_reg);
          break;
        default:
          UNREACHABLE();
          break;
        }
      return;
    }
    case kInt32x4Not:
    case kInt32x4Neg: {
      DCHECK(instr->hydrogen()->value()->representation().IsInt32x4());
      XMMRegister input_reg = ToInt32x4Register(instr->value());
      switch (instr->op()) {
        case kInt32x4Not:
          __ notps(input_reg);
          break;
        case kInt32x4Neg:
          __ pnegd(input_reg);
          break;
        default:
          UNREACHABLE();
          break;
      }
      return;
    }
    case kFloat32x4BitsToInt32x4:
    case kFloat32x4ToInt32x4: {
      DCHECK(instr->hydrogen()->value()->representation().IsFloat32x4());
      XMMRegister input_reg = ToFloat32x4Register(instr->value());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      if (instr->op() == kFloat32x4BitsToInt32x4) {
        if (!result_reg.is(input_reg)) {
          __ movaps(result_reg, input_reg);
        }
      } else {
        DCHECK(instr->op() == kFloat32x4ToInt32x4);
        __ cvtps2dq(result_reg, input_reg);
      }
      return;
    }
    case kInt32x4BitsToFloat32x4:
    case kInt32x4ToFloat32x4: {
      DCHECK(instr->hydrogen()->value()->representation().IsInt32x4());
      XMMRegister input_reg = ToInt32x4Register(instr->value());
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      if (instr->op() == kInt32x4BitsToFloat32x4) {
        if (!result_reg.is(input_reg)) {
          __ movaps(result_reg, input_reg);
        }
      } else {
        DCHECK(instr->op() == kInt32x4ToFloat32x4);
        __ cvtdq2ps(result_reg, input_reg);
      }
      return;
    }
    case kFloat32x4Splat: {
      DCHECK(instr->hydrogen()->value()->representation().IsDouble());
      XMMRegister input_reg = ToDoubleRegister(instr->value());
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      XMMRegister xmm_scratch = xmm0;
      __ xorps(xmm_scratch, xmm_scratch);
      __ cvtsd2ss(xmm_scratch, input_reg);
      __ shufps(xmm_scratch, xmm_scratch, 0x0);
      __ movaps(result_reg, xmm_scratch);
      return;
    }
    case kInt32x4Splat: {
      DCHECK(instr->hydrogen()->value()->representation().IsInteger32());
      Register input_reg = ToRegister(instr->value());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      __ movd(result_reg, input_reg);
      __ shufps(result_reg, result_reg, 0x0);
      return;
    }
    case kInt32x4GetSignMask: {
      DCHECK(instr->hydrogen()->value()->representation().IsInt32x4());
      XMMRegister input_reg = ToInt32x4Register(instr->value());
      Register result = ToRegister(instr->result());
      __ movmskps(result, input_reg);
      return;
    }
    case kFloat32x4GetSignMask: {
      DCHECK(instr->hydrogen()->value()->representation().IsFloat32x4());
      XMMRegister input_reg = ToFloat32x4Register(instr->value());
      Register result = ToRegister(instr->result());
      __ movmskps(result, input_reg);
      return;
    }
    case kFloat64x2GetSignMask: {
      DCHECK(instr->hydrogen()->value()->representation().IsFloat64x2());
      XMMRegister input_reg = ToFloat64x2Register(instr->value());
      Register result = ToRegister(instr->result());
      __ movmskpd(result, input_reg);
      return;
    }
    case kInt32x4GetFlagX:
    case kInt32x4GetFlagY:
    case kInt32x4GetFlagZ:
    case kInt32x4GetFlagW: {
      DCHECK(instr->hydrogen()->value()->representation().IsInt32x4());
      bool flag = false;
      switch (instr->op()) {
        case kInt32x4GetFlagX:
          flag = true;
          break;
        case kInt32x4GetFlagY:
          flag = true;
          select = 0x1;
          break;
        case kInt32x4GetFlagZ:
          flag = true;
          select = 0x2;
          break;
        case kInt32x4GetFlagW:
          flag = true;
          select = 0x3;
          break;
        default:
          UNREACHABLE();
      }

      XMMRegister input_reg = ToInt32x4Register(instr->value());
      Register result = ToRegister(instr->result());
      if (select == 0x0) {
        __ movd(result, input_reg);
      } else {
        if (CpuFeatures::IsSupported(SSE4_1)) {
          CpuFeatureScope scope(masm(), SSE4_1);
          __ extractps(result, input_reg, select);
        } else {
          XMMRegister xmm_scratch = xmm0;
          __ pshufd(xmm_scratch, input_reg, select);
          __ movd(result, xmm_scratch);
        }
      }

      if (flag) {
        Label false_value, done;
        __ test(result, result);
        __ j(zero, &false_value, Label::kNear);
        __ LoadRoot(result, Heap::kTrueValueRootIndex);
        __ jmp(&done, Label::kNear);
        __ bind(&false_value);
        __ LoadRoot(result, Heap::kFalseValueRootIndex);
        __ bind(&done);
      }
      return;
    }
    default:
      UNREACHABLE();
      return;
  }
}


void LCodeGen::DoBinarySIMDOperation(LBinarySIMDOperation* instr) {
  uint8_t imm8 = 0;  // for with operation
  switch (instr->op()) {
    case kFloat32x4ExtractLane: {
      DCHECK(instr->hydrogen()->left()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsInteger32());
      if (instr->hydrogen()->right()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->right())->HasInteger32Value()) {
        uint32_t right = ToInteger32(LConstantOperand::cast(instr->right()));
        DCHECK((right >= 0) && (right <= 3));
        XMMRegister left_reg = ToFloat32x4Register(instr->left());
        XMMRegister result = ToDoubleRegister(instr->result());
        XMMRegister xmm_scratch = result.is(left_reg) ? xmm0 : result;
        switch (right) {
          case 3:
            imm8++;
          case 2:
            imm8++;
          case 1:
            imm8++;
          case 0:
            break;
        }
        if (imm8 == 0x0) {
          __ xorps(xmm_scratch, xmm_scratch);
          __ cvtss2sd(xmm_scratch, left_reg);
          if (!xmm_scratch.is(result)) {
            __ movaps(result, xmm_scratch);
          }
        } else {
          __ pshufd(xmm_scratch, left_reg, imm8);
          if (!xmm_scratch.is(result)) {
            __ xorps(result, result);
          }
          __ cvtss2sd(result, xmm_scratch);
        }
        return;
     } else {
       Comment(";;; deoptimize: non-constant selector for extractLane");
       DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
       return;
     }
    }
    case kInt32x4ExtractLane: {
      DCHECK(instr->hydrogen()->left()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsInteger32());
      if (instr->hydrogen()->right()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->right())->HasInteger32Value()) {
        XMMRegister left_reg = ToInt32x4Register(instr->left());
        uint32_t right = ToInteger32(LConstantOperand::cast(instr->right()));
        DCHECK((right >= 0) && (right <= 3));
        Register result = ToRegister(instr->result());
        switch (right) {
          case 3:
            imm8 = 0x3;
            break;
          case 2:
            imm8 = 0x2;
            break;
          case 1:
            imm8 = 0x1;
            break;
          case 0:
            imm8 = 0x0;
            break;
          default:
            UNREACHABLE();
        }
        if (imm8 == 0x0) {
          __ movd(result, left_reg);
        } else {
          if (CpuFeatures::IsSupported(SSE4_1)) {
            CpuFeatureScope scope(masm(), SSE4_1);
            __ extractps(result, left_reg, imm8);
          } else {
            XMMRegister xmm_scratch = xmm0;
            __ pshufd(xmm_scratch, left_reg, imm8);
            __ movd(result, xmm_scratch);
          }
        }
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for extractLane");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kFloat64x2ExtractLane: {
      DCHECK(instr->hydrogen()->left()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->right()->representation().IsInteger32());
      if (instr->hydrogen()->right()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->right())->HasInteger32Value()) {
        uint32_t right = ToInteger32(LConstantOperand::cast(instr->right()));
        DCHECK((right >= 0) && (right <= 1));
        XMMRegister left_reg = ToFloat64x2Register(instr->left());
        XMMRegister result = ToDoubleRegister(instr->result());
        if (!left_reg.is(result)) {
          __ movaps(result, left_reg);
        }
        if (right == 1) {
          __ shufpd(result, left_reg, 0x1);
        }
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for extractLane");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kFloat32x4Add:
    case kFloat32x4Sub:
    case kFloat32x4Mul:
    case kFloat32x4Div:
    case kFloat32x4Min:
    case kFloat32x4MinNum:
    case kFloat32x4Max:
    case kFloat32x4MaxNum: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsFloat32x4());
      XMMRegister left_reg = ToFloat32x4Register(instr->left());
      XMMRegister right_reg = ToFloat32x4Register(instr->right());
      switch (instr->op()) {
        case kFloat32x4Add:
          __ addps(left_reg, right_reg);
          break;
        case kFloat32x4Sub:
          __ subps(left_reg, right_reg);
          break;
        case kFloat32x4Mul:
          __ mulps(left_reg, right_reg);
          break;
        case kFloat32x4Div:
          __ divps(left_reg, right_reg);
          break;
        case kFloat32x4Min:
        case kFloat32x4MinNum:
          __ minps(left_reg, right_reg);
          break;
        case kFloat32x4Max:
        case kFloat32x4MaxNum:
          __ maxps(left_reg, right_reg);
          break;
        default:
          UNREACHABLE();
          break;
      }
      return;
    }
    case kFloat32x4Scale: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsDouble());
      XMMRegister left_reg = ToFloat32x4Register(instr->left());
      XMMRegister right_reg = ToDoubleRegister(instr->right());
      XMMRegister scratch_reg = xmm0;
      __ xorps(scratch_reg, scratch_reg);
      __ cvtsd2ss(scratch_reg, right_reg);
      __ shufps(scratch_reg, scratch_reg, 0x0);
      __ mulps(left_reg, scratch_reg);
      return;
    }
    case kFloat64x2Add:
    case kFloat64x2Sub:
    case kFloat64x2Mul:
    case kFloat64x2Div:
    case kFloat64x2Min:
    case kFloat64x2Max: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->right()->representation().IsFloat64x2());
      XMMRegister left_reg = ToFloat64x2Register(instr->left());
      XMMRegister right_reg = ToFloat64x2Register(instr->right());
      switch (instr->op()) {
        case kFloat64x2Add:
          __ addpd(left_reg, right_reg);
          break;
        case kFloat64x2Sub:
          __ subpd(left_reg, right_reg);
          break;
        case kFloat64x2Mul:
          __ mulpd(left_reg, right_reg);
          break;
        case kFloat64x2Div:
          __ divpd(left_reg, right_reg);
          break;
        case kFloat64x2Min:
          __ minpd(left_reg, right_reg);
          break;
        case kFloat64x2Max:
          __ maxpd(left_reg, right_reg);
          break;
        default:
          UNREACHABLE();
          break;
      }
      return;
    }
    case kFloat64x2Scale: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->right()->representation().IsDouble());
      XMMRegister left_reg = ToFloat64x2Register(instr->left());
      XMMRegister right_reg = ToDoubleRegister(instr->right());
      __ shufpd(right_reg, right_reg, 0x0);
      __ mulpd(left_reg, right_reg);
      return;
    }
    case kInt32x4ShiftLeft:
    case kInt32x4ShiftRight:
    case kInt32x4ShiftRightArithmetic: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsInt32x4());
      if (instr->hydrogen()->right()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->right())->HasInteger32Value()) {
        int32_t value = ToInteger32(LConstantOperand::cast(instr->right()));
        uint8_t shift = static_cast<uint8_t>(value & 0xFF);
        XMMRegister left_reg = ToInt32x4Register(instr->left());
        switch (instr->op()) {
          case kInt32x4ShiftLeft:
            __ pslld(left_reg, shift);
            break;
          case kInt32x4ShiftRight:
            __ psrld(left_reg, shift);
            break;
          case kInt32x4ShiftRightArithmetic:
            __ psrad(left_reg, shift);
            break;
          default:
            UNREACHABLE();
        }
        return;
      } else {
        XMMRegister left_reg = ToInt32x4Register(instr->left());
        Register shift = ToRegister(instr->right());
        XMMRegister xmm_scratch = double_scratch0();
        __ movd(xmm_scratch, shift);
        switch (instr->op()) {
          case kInt32x4ShiftLeft:
            __ pslld(left_reg, xmm_scratch);
            break;
          case kInt32x4ShiftRight:
            __ psrld(left_reg, xmm_scratch);
            break;
          case kInt32x4ShiftRightArithmetic:
            __ psrad(left_reg, xmm_scratch);
            break;
          default:
            UNREACHABLE();
        }
        return;
      }
    }
    case kFloat32x4LessThan:
    case kFloat32x4LessThanOrEqual:
    case kFloat32x4Equal:
    case kFloat32x4NotEqual:
    case kFloat32x4GreaterThanOrEqual:
    case kFloat32x4GreaterThan: {
      DCHECK(instr->hydrogen()->left()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsFloat32x4());
      XMMRegister left_reg = ToFloat32x4Register(instr->left());
      XMMRegister right_reg = ToFloat32x4Register(instr->right());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      switch (instr->op()) {
        case kFloat32x4LessThan:
          if (result_reg.is(left_reg)) {
            __ cmpltps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpnltps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpltps(result_reg, right_reg);
          }
          break;
        case kFloat32x4LessThanOrEqual:
          if (result_reg.is(left_reg)) {
            __ cmpleps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpnleps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpleps(result_reg, right_reg);
          }
          break;
        case kFloat32x4Equal:
          if (result_reg.is(left_reg)) {
            __ cmpeqps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpeqps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpeqps(result_reg, right_reg);
          }
          break;
        case kFloat32x4NotEqual:
          if (result_reg.is(left_reg)) {
            __ cmpneqps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpneqps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpneqps(result_reg, right_reg);
          }
          break;
        case kFloat32x4GreaterThanOrEqual:
          if (result_reg.is(left_reg)) {
            __ cmpnltps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpltps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpnltps(result_reg, right_reg);
          }
          break;
        case kFloat32x4GreaterThan:
          if (result_reg.is(left_reg)) {
            __ cmpnleps(result_reg, right_reg);
          } else if (result_reg.is(right_reg)) {
            __ cmpleps(result_reg, left_reg);
          } else {
            __ movaps(result_reg, left_reg);
            __ cmpnleps(result_reg, right_reg);
          }
          break;
        default:
          UNREACHABLE();
          break;
      }
      return;
    }
    case kInt32x4And:
    case kInt32x4Or:
    case kInt32x4Xor:
    case kInt32x4Add:
    case kInt32x4Sub:
    case kInt32x4Mul:
    case kInt32x4GreaterThan:
    case kInt32x4Equal:
    case kInt32x4LessThan: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsInt32x4());
      XMMRegister left_reg = ToInt32x4Register(instr->left());
      XMMRegister right_reg = ToInt32x4Register(instr->right());
      switch (instr->op()) {
        case kInt32x4And:
          __ andps(left_reg, right_reg);
          break;
        case kInt32x4Or:
          __ orps(left_reg, right_reg);
          break;
        case kInt32x4Xor:
          __ xorps(left_reg, right_reg);
          break;
        case kInt32x4Add:
          __ paddd(left_reg, right_reg);
          break;
        case kInt32x4Sub:
          __ psubd(left_reg, right_reg);
          break;
        case kInt32x4Mul:
          if (CpuFeatures::IsSupported(SSE4_1)) {
            CpuFeatureScope scope(masm(), SSE4_1);
            __ pmulld(left_reg, right_reg);
          } else {
            // The algorithm is from http://stackoverflow.com/questions/10500766/sse-multiplication-of-4-32-bit-integers
            XMMRegister xmm_scratch = xmm0;
            __ movaps(xmm_scratch, left_reg);
            __ pmuludq(left_reg, right_reg);
            __ psrldq(xmm_scratch, 4);
            __ psrldq(right_reg, 4);
            __ pmuludq(xmm_scratch, right_reg);
            __ pshufd(left_reg, left_reg, 8);
            __ pshufd(xmm_scratch, xmm_scratch, 8);
            __ punpackldq(left_reg, xmm_scratch);
          }
          break;
        case kInt32x4GreaterThan:
          __ pcmpgtd(left_reg, right_reg);
          break;
        case kInt32x4Equal:
          __ pcmpeqd(left_reg, right_reg);
          break;
        case kInt32x4LessThan: {
          XMMRegister xmm_scratch = xmm0;
          __ movaps(xmm_scratch, right_reg);
          __ pcmpgtd(xmm_scratch, left_reg);
          __ movaps(left_reg, xmm_scratch);
          break;
        }
        default:
          UNREACHABLE();
          break;
      }
      return;
    }
    case kFloat64x2Constructor: {
      DCHECK(instr->hydrogen()->left()->representation().IsDouble());
      DCHECK(instr->hydrogen()->right()->representation().IsDouble());
      XMMRegister left_reg = ToDoubleRegister(instr->left());
      XMMRegister right_reg = ToDoubleRegister(instr->right());
      XMMRegister result_reg = ToFloat64x2Register(instr->result());
      __ sub(esp, Immediate(kFloat64x2Size));
      __ movsd(Operand(esp, 0 * kDoubleSize), left_reg);
      __ movsd(Operand(esp, 1 * kDoubleSize), right_reg);
      __ movups(result_reg, Operand(esp, 0));
      __ add(esp, Immediate(kFloat64x2Size));
      return;
    }
    case kInt32x4WithFlagW:
      imm8++;
    case kInt32x4WithFlagZ:
      imm8++;
    case kInt32x4WithFlagY:
      imm8++;
    case kInt32x4WithFlagX: {
      DCHECK(instr->left()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->left()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->right()->representation().IsTagged());
      HType type = instr->hydrogen()->right()->type();
      XMMRegister left_reg = ToInt32x4Register(instr->left());
      Register right_reg = ToRegister(instr->right());
      Label load_false_value, done;
      if (type.IsBoolean()) {
        __ sub(esp, Immediate(kInt32x4Size));
        __ movups(Operand(esp, 0), left_reg);
        __ CompareRoot(right_reg, Heap::kTrueValueRootIndex);
        __ j(not_equal, &load_false_value, Label::kNear);
     } else {
        Comment(";;; deoptimize: other types for int32x4.withFlagX/Y/Z/W.");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
     }
      // load true value.
      __ mov(Operand(esp, imm8 * kFloatSize), Immediate(0xFFFFFFFF));
      __ jmp(&done, Label::kNear);
      __ bind(&load_false_value);
      __ mov(Operand(esp, imm8 * kFloatSize), Immediate(0x0));
      __ bind(&done);
      __ movups(left_reg, Operand(esp, 0));
      __ add(esp, Immediate(kInt32x4Size));
      return;
    }
    default:
      UNREACHABLE();
      return;
  }
}


static uint8_t ComputeShuffleSelect64x2(uint32_t x, uint32_t y) {
  DCHECK(x < 2 && y < 2);
  uint32_t r = static_cast<uint8_t>(
      ((y << 1) | (x << 0)) & 0xFF);
  return r;
}


void LCodeGen::DoTernarySIMDOperation(LTernarySIMDOperation* instr) {
  uint8_t imm8 = 0;
  switch (instr->op()) {
    case kFloat32x4Select: {
      DCHECK(instr->hydrogen()->first()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->second()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->third()->representation().IsFloat32x4());

      XMMRegister mask_reg = ToInt32x4Register(instr->first());
      XMMRegister left_reg = ToFloat32x4Register(instr->second());
      XMMRegister right_reg = ToFloat32x4Register(instr->third());
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      XMMRegister temp_reg = xmm0;

      // Copy mask.
      __ movaps(temp_reg, mask_reg);
      // Invert it.
      __ notps(temp_reg);
      // temp_reg = temp_reg & falseValue.
      __ andps(temp_reg, right_reg);

      if (!result_reg.is(mask_reg)) {
        if (result_reg.is(left_reg)) {
          // result_reg = result_reg & trueValue.
          __ andps(result_reg, mask_reg);
          // out = result_reg | temp_reg.
          __ orps(result_reg, temp_reg);
        } else {
          __ movaps(result_reg, mask_reg);
          // result_reg = result_reg & trueValue.
          __ andps(result_reg, left_reg);
          // out = result_reg | temp_reg.
          __ orps(result_reg, temp_reg);
        }
      } else {
        // result_reg = result_reg & trueValue.
        __ andps(result_reg, left_reg);
        // out = result_reg | temp_reg.
        __ orps(result_reg, temp_reg);
      }
      return;
    }
    case kInt32x4Select: {
      DCHECK(instr->hydrogen()->first()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->second()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->third()->representation().IsInt32x4());

      XMMRegister mask_reg = ToInt32x4Register(instr->first());
      XMMRegister left_reg = ToInt32x4Register(instr->second());
      XMMRegister right_reg = ToInt32x4Register(instr->third());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      XMMRegister temp_reg = xmm0;

      // Copy mask.
      __ movaps(temp_reg, mask_reg);
      // Invert it.
      __ notps(temp_reg);
      // temp_reg = temp_reg & falseValue.
      __ andps(temp_reg, right_reg);

      if (!result_reg.is(mask_reg)) {
        if (result_reg.is(left_reg)) {
          // result_reg = result_reg & trueValue.
          __ andps(result_reg, mask_reg);
          // out = result_reg | temp_reg.
          __ orps(result_reg, temp_reg);
        } else {
          __ movaps(result_reg, mask_reg);
          // result_reg = result_reg & trueValue.
          __ andps(result_reg, left_reg);
          // out = result_reg | temp_reg.
          __ orps(result_reg, temp_reg);
        }
      } else {
        // result_reg = result_reg & trueValue.
        __ andps(result_reg, left_reg);
        // out = result_reg | temp_reg.
        __ orps(result_reg, temp_reg);
      }
      return;
    }
    case kFloat32x4Clamp: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->second()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->third()->representation().IsFloat32x4());

      XMMRegister value_reg = ToFloat32x4Register(instr->first());
      XMMRegister lower_reg = ToFloat32x4Register(instr->second());
      XMMRegister upper_reg = ToFloat32x4Register(instr->third());
      __ minps(value_reg, upper_reg);
      __ maxps(value_reg, lower_reg);
      return;
    }
    case kFloat64x2Clamp: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->second()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->third()->representation().IsFloat64x2());

      XMMRegister value_reg = ToFloat64x2Register(instr->first());
      XMMRegister lower_reg = ToFloat64x2Register(instr->second());
      XMMRegister upper_reg = ToFloat64x2Register(instr->third());
      __ minpd(value_reg, upper_reg);
      __ maxpd(value_reg, lower_reg);
      return;
    }
    case kFloat64x2Swizzle: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsFloat64x2());
      if ((instr->hydrogen()->second()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->second())->HasInteger32Value()) &&
          (instr->hydrogen()->third()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->third())->HasInteger32Value())) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->second()));
        int32_t y = ToInteger32(LConstantOperand::cast(instr->third()));
        uint8_t select = ComputeShuffleSelect64x2(x, y);
        XMMRegister left_reg = ToFloat64x2Register(instr->first());
        __ shufpd(left_reg, left_reg, select);
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for swizzle");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kFloat32x4ReplaceLane: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsFloat32x4());
      DCHECK(instr->hydrogen()->second()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->third()->representation().IsDouble());
      if (instr->hydrogen()->second()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->second())->HasInteger32Value()) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->second()));
        DCHECK((x >= 0) && (x <= 3));
        switch (x) {
          case 3: imm8++;
          case 2: imm8++;
          case 1: imm8++;
          case 0: break;
        }
        XMMRegister result_reg = ToFloat32x4Register(instr->first());
        XMMRegister value_reg = ToDoubleRegister(instr->third());
        XMMRegister xmm_scratch = xmm0;
        __ xorps(xmm_scratch, xmm_scratch);
        __ cvtsd2ss(xmm_scratch, value_reg);
        if (CpuFeatures::IsSupported(SSE4_1)) {
          imm8 = imm8 << 4;
          CpuFeatureScope scope(masm(), SSE4_1);
          __ insertps(result_reg, xmm_scratch, imm8);
        } else {
          __ sub(esp, Immediate(kFloat32x4Size));
          __ movups(Operand(esp, 0), result_reg);
          __ movss(Operand(esp, imm8 * kFloatSize), xmm_scratch);
          __ movups(result_reg, Operand(esp, 0));
          __ add(esp, Immediate(kFloat32x4Size));
        }
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for replaceLane.");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kFloat64x2ReplaceLane: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->second()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->third()->representation().IsDouble());
      if (instr->hydrogen()->second()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->second())->HasInteger32Value()) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->second()));
        DCHECK((x >= 0) && (x <= 2));
        switch (x) {
          case 1: imm8++;
          case 0: break;
        }
        XMMRegister result_reg = ToFloat64x2Register(instr->first());
        XMMRegister value_reg = ToDoubleRegister(instr->third());
        __ sub(esp, Immediate(kFloat64x2Size));
        __ movups(Operand(esp, 0), result_reg);
        __ movsd(Operand(esp, x * kDoubleSize), value_reg);
        __ movups(result_reg, Operand(esp, 0));
        __ add(esp, Immediate(kFloat64x2Size));
        return;
    } else {
        Comment(";;; deoptimize: non-constant selector for replaceLane.");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kInt32x4ReplaceLane: {
      DCHECK(instr->first()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->first()->representation().IsInt32x4());
      DCHECK(instr->hydrogen()->second()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->third()->representation().IsInteger32());
      if (instr->hydrogen()->second()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->second())->HasInteger32Value()) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->second()));
        DCHECK((x >= 0) && (x <= 4));
        switch (x) {
          case 3: imm8++;
          case 2: imm8++;
          case 1: imm8++;
          case 0: break;
        }
        XMMRegister result_reg = ToInt32x4Register(instr->first());
        Register value_reg = ToRegister(instr->third());
        if (CpuFeatures::IsSupported(SSE4_1)) {
          CpuFeatureScope scope(masm(), SSE4_1);
          __ pinsrd(result_reg, value_reg, imm8);
        } else {
          __ sub(esp, Immediate(kInt32x4Size));
          __ movdqu(Operand(esp, 0), result_reg);
          __ mov(Operand(esp, imm8 * kFloatSize), value_reg);
          __ movdqu(result_reg, Operand(esp, 0));
          __ add(esp, Immediate(kInt32x4Size));
        }
        return;
    } else {
        Comment(";;; deoptimize: non-constant selector for replaceLane.");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    default:
      UNREACHABLE();
      return;
  }
}


void LCodeGen::DoQuarternarySIMDOperation(LQuarternarySIMDOperation* instr) {
  switch (instr->op()) {
    case kFloat32x4Constructor: {
      DCHECK(instr->hydrogen()->x()->representation().IsDouble());
      DCHECK(instr->hydrogen()->y()->representation().IsDouble());
      DCHECK(instr->hydrogen()->z()->representation().IsDouble());
      DCHECK(instr->hydrogen()->w()->representation().IsDouble());
      XMMRegister x_reg = ToDoubleRegister(instr->x());
      XMMRegister y_reg = ToDoubleRegister(instr->y());
      XMMRegister z_reg = ToDoubleRegister(instr->z());
      XMMRegister w_reg = ToDoubleRegister(instr->w());
      XMMRegister result_reg = ToFloat32x4Register(instr->result());
      __ sub(esp, Immediate(kFloat32x4Size));
      __ xorps(xmm0, xmm0);
      __ cvtsd2ss(xmm0, x_reg);
      __ movss(Operand(esp, 0 * kFloatSize), xmm0);
      __ xorps(xmm0, xmm0);
      __ cvtsd2ss(xmm0, y_reg);
      __ movss(Operand(esp, 1 * kFloatSize), xmm0);
      __ xorps(xmm0, xmm0);
      __ cvtsd2ss(xmm0, z_reg);
      __ movss(Operand(esp, 2 * kFloatSize), xmm0);
      __ xorps(xmm0, xmm0);
      __ cvtsd2ss(xmm0, w_reg);
      __ movss(Operand(esp, 3 * kFloatSize), xmm0);
      __ movups(result_reg, Operand(esp, 0 * kFloatSize));
      __ add(esp, Immediate(kFloat32x4Size));
      return;
    }
    case kInt32x4Constructor: {
      DCHECK(instr->hydrogen()->x()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->y()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->z()->representation().IsInteger32());
      DCHECK(instr->hydrogen()->w()->representation().IsInteger32());
      Register x_reg = ToRegister(instr->x());
      Register y_reg = ToRegister(instr->y());
      Register z_reg = ToRegister(instr->z());
      Register w_reg = ToRegister(instr->w());
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      __ sub(esp, Immediate(kInt32x4Size));
      __ mov(Operand(esp, 0 * kInt32Size), x_reg);
      __ mov(Operand(esp, 1 * kInt32Size), y_reg);
      __ mov(Operand(esp, 2 * kInt32Size), z_reg);
      __ mov(Operand(esp, 3 * kInt32Size), w_reg);
      __ movups(result_reg, Operand(esp, 0 * kInt32Size));
      __ add(esp, Immediate(kInt32x4Size));
      return;
    }
    case kInt32x4Bool: {
      DCHECK(instr->hydrogen()->x()->representation().IsTagged());
      DCHECK(instr->hydrogen()->y()->representation().IsTagged());
      DCHECK(instr->hydrogen()->z()->representation().IsTagged());
      DCHECK(instr->hydrogen()->w()->representation().IsTagged());
      HType x_type = instr->hydrogen()->x()->type();
      HType y_type = instr->hydrogen()->y()->type();
      HType z_type = instr->hydrogen()->z()->type();
      HType w_type = instr->hydrogen()->w()->type();
      if (!x_type.IsBoolean() || !y_type.IsBoolean() ||
          !z_type.IsBoolean() || !w_type.IsBoolean()) {
        Comment(";;; deoptimize: other types for int32x4.bool.");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
      XMMRegister result_reg = ToInt32x4Register(instr->result());
      Register x_reg = ToRegister(instr->x());
      Register y_reg = ToRegister(instr->y());
      Register z_reg = ToRegister(instr->z());
      Register w_reg = ToRegister(instr->w());
      Label load_false_x, done_x, load_false_y, done_y,
            load_false_z, done_z, load_false_w, done_w;
      __ sub(esp, Immediate(kInt32x4Size));

      __ CompareRoot(x_reg, Heap::kTrueValueRootIndex);
      __ j(not_equal, &load_false_x, Label::kNear);
      __ mov(Operand(esp, 0 * kInt32Size), Immediate(-1));
      __ jmp(&done_x, Label::kNear);
      __ bind(&load_false_x);
      __ mov(Operand(esp, 0 * kInt32Size), Immediate(0x0));
      __ bind(&done_x);

      __ CompareRoot(y_reg, Heap::kTrueValueRootIndex);
      __ j(not_equal, &load_false_y, Label::kNear);
      __ mov(Operand(esp, 1 * kInt32Size), Immediate(-1));
      __ jmp(&done_y, Label::kNear);
      __ bind(&load_false_y);
      __ mov(Operand(esp, 1 * kInt32Size), Immediate(0x0));
      __ bind(&done_y);

      __ CompareRoot(z_reg, Heap::kTrueValueRootIndex);
      __ j(not_equal, &load_false_z, Label::kNear);
      __ mov(Operand(esp, 2 * kInt32Size), Immediate(-1));
      __ jmp(&done_z, Label::kNear);
      __ bind(&load_false_z);
      __ mov(Operand(esp, 2 * kInt32Size), Immediate(0x0));
      __ bind(&done_z);

      __ CompareRoot(w_reg, Heap::kTrueValueRootIndex);
      __ j(not_equal, &load_false_w, Label::kNear);
      __ mov(Operand(esp, 3 * kInt32Size), Immediate(-1));
      __ jmp(&done_w, Label::kNear);
      __ bind(&load_false_w);
      __ mov(Operand(esp, 3 * kInt32Size), Immediate(0x0));
      __ bind(&done_w);

      __ movups(result_reg, Operand(esp, 0));
      __ add(esp, Immediate(kInt32x4Size));
      return;
    }
    case kFloat64x2Shuffle: {
      DCHECK(instr->hydrogen()->x()->representation().IsFloat64x2());
      DCHECK(instr->hydrogen()->y()->representation().IsFloat64x2());

      if ((instr->hydrogen()->z()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->z())->HasInteger32Value()) &&
          (instr->hydrogen()->w()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->w())->HasInteger32Value())) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->z()));
        int32_t y = ToInteger32(LConstantOperand::cast(instr->w()));
        XMMRegister lhs = ToFloat64x2Register(instr->x());
        XMMRegister rhs = ToFloat64x2Register(instr->y());
        XMMRegister temp = xmm0;

        uint32_t num_lanes_from_lhs = (x < 2) + (y < 2);
        if (num_lanes_from_lhs  == 2) {
          uint8_t select = ComputeShuffleSelect64x2(x, y);
          __ shufpd(lhs, lhs, select);
          return;
        } else if (num_lanes_from_lhs == 0) {
          x -= 2;
          y -= 2;
          uint8_t select = ComputeShuffleSelect64x2(x, y);
          __ movaps(lhs, rhs);
          __ shufpd(lhs, lhs, select);
          return;
        } else if (num_lanes_from_lhs == 1) {
          if (x < 2) {
            uint8_t select = ComputeShuffleSelect64x2(x, y%2);
            __ shufpd(lhs, rhs, select);
            return;
          } else {
            uint8_t select = ComputeShuffleSelect64x2(x%2, y);
            __ movaps(temp, rhs);
            __ shufpd(temp, lhs, select);
            __ movaps(lhs, temp);
            return;
          }
        }
      } else {
        Comment(";;; deoptimize: non-constant selector for shuffle");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    default:
      UNREACHABLE();
      return;
  }
}


static uint8_t ComputeShuffleSelect(uint32_t x, uint32_t y,
                                  uint32_t z, uint32_t w) {
  DCHECK(x < 4 && y < 4 && z < 4 && w < 4);
  uint32_t r = static_cast<uint8_t>(
      ((w << 6) | (z << 4) | (y << 2) | (x << 0)) & 0xFF);
  return r;
}


void LCodeGen::DoQuinarySIMDOperation(LQuinarySIMDOperation* instr) {
  switch (instr->op()) {
    case kFloat32x4Swizzle: {
      DCHECK(instr->a0()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->a0()->representation().IsFloat32x4());
      if ((instr->hydrogen()->a1()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a1())->HasInteger32Value()) &&
          (instr->hydrogen()->a2()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a2())->HasInteger32Value()) &&
          (instr->hydrogen()->a3()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a3())->HasInteger32Value()) &&
          (instr->hydrogen()->a4()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a4())->HasInteger32Value())) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->a1()));
        int32_t y = ToInteger32(LConstantOperand::cast(instr->a2()));
        int32_t z = ToInteger32(LConstantOperand::cast(instr->a3()));
        int32_t w = ToInteger32(LConstantOperand::cast(instr->a4()));
        uint8_t select = ComputeShuffleSelect(x, y, z, w);
        XMMRegister left_reg = ToFloat32x4Register(instr->a0());
        __ shufps(left_reg, left_reg, select);
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for swizzle");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    case kInt32x4Swizzle: {
      DCHECK(instr->a0()->Equals(instr->result()));
      DCHECK(instr->hydrogen()->a0()->representation().IsInt32x4());
      if ((instr->hydrogen()->a1()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a1())->HasInteger32Value()) &&
          (instr->hydrogen()->a2()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a2())->HasInteger32Value()) &&
          (instr->hydrogen()->a3()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a3())->HasInteger32Value()) &&
          (instr->hydrogen()->a4()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a4())->HasInteger32Value())) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->a1()));
        int32_t y = ToInteger32(LConstantOperand::cast(instr->a2()));
        int32_t z = ToInteger32(LConstantOperand::cast(instr->a3()));
        int32_t w = ToInteger32(LConstantOperand::cast(instr->a4()));
        uint8_t select = ComputeShuffleSelect(x, y, z, w);
        XMMRegister left_reg = ToInt32x4Register(instr->a0());
        __ pshufd(left_reg, left_reg, select);
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for shuffle");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }
    default:
      UNREACHABLE();
      return;
  }
}


void LCodeGen::DoSenarySIMDOperation(LSenarySIMDOperation* instr) {
  switch (instr->op()) {
    case kFloat32x4Shuffle:
    case kInt32x4Shuffle: {
      DCHECK(instr->a0()->Equals(instr->result()));
      if (instr->op() == kFloat32x4Shuffle) {
        DCHECK(instr->hydrogen()->a0()->representation().IsFloat32x4());
        DCHECK(instr->hydrogen()->a1()->representation().IsFloat32x4());
      } else {
        DCHECK(instr->hydrogen()->a0()->representation().IsInt32x4());
        DCHECK(instr->hydrogen()->a1()->representation().IsInt32x4());
      }

      if ((instr->hydrogen()->a2()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a2())->HasInteger32Value()) &&
          (instr->hydrogen()->a3()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a3())->HasInteger32Value()) &&
          (instr->hydrogen()->a4()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a4())->HasInteger32Value()) &&
          (instr->hydrogen()->a5()->IsConstant() &&
          HConstant::cast(instr->hydrogen()->a5())->HasInteger32Value())) {
        int32_t x = ToInteger32(LConstantOperand::cast(instr->a2()));
        int32_t y = ToInteger32(LConstantOperand::cast(instr->a3()));
        int32_t z = ToInteger32(LConstantOperand::cast(instr->a4()));
        int32_t w = ToInteger32(LConstantOperand::cast(instr->a5()));
         XMMRegister lhs, rhs;
        if (instr->op() == kFloat32x4Shuffle) {
          lhs = ToFloat32x4Register(instr->a0());
          rhs = ToFloat32x4Register(instr->a1());
        } else {
          lhs = ToInt32x4Register(instr->a0());
          rhs = ToInt32x4Register(instr->a1());
        }
        XMMRegister temp = xmm0;

        uint32_t num_lanes_from_lhs = (x < 4) + (y < 4) + (z < 4) + (w < 4);
        if (num_lanes_from_lhs == 4) {
          uint8_t select = ComputeShuffleSelect(x, y, z, w);
          __ shufps(lhs, lhs, select);
          return;
        } else if (num_lanes_from_lhs == 0) {
          x -= 4;
          y -= 4;
          z -= 4;
          w -= 4;
          uint8_t select = ComputeShuffleSelect(x, y, z, w);
          __ movaps(lhs, rhs);
          __ shufps(lhs, lhs, select);
          return;
        } else if (num_lanes_from_lhs == 3) {
          uint8_t first_select = 0xFF;
          uint8_t second_select = 0xFF;
          if (x < 4 && y < 4) {
            if (w >= 4) {
                w -= 4;
                // T = (Rw Rw Lz Lz) = shufps(firstMask, lhs, rhs)
                first_select = ComputeShuffleSelect(w, w, z, z);
                // (Lx Ly Lz Rw) = (Lx Ly Tz Tx) = shufps(secondMask, T, lhs)
                second_select = ComputeShuffleSelect(x, y, 2, 0);
            } else {
                DCHECK(z >= 4);
                z -= 4;
                // T = (Rz Rz Lw Lw) = shufps(firstMask, lhs, rhs)
                first_select = ComputeShuffleSelect(z, z, w, w);
                // (Lx Ly Rz Lw) = (Lx Ly Tx Tz) = shufps(secondMask, T, lhs)
                second_select = ComputeShuffleSelect(x, y, 0, 2);
            }

            __ movaps(temp, rhs);
            __ shufps(temp, lhs, first_select);
            __ shufps(lhs, temp, second_select);
            return;
          }

          DCHECK(z < 4 && w < 4);

          if (y >= 4) {
              y -= 4;
              // T = (Ry Ry Lx Lx) = shufps(firstMask, lhs, rhs)
              first_select = ComputeShuffleSelect(y, y, x, x);
              // (Lx Ry Lz Lw) = (Tz Tx Lz Lw) = shufps(secondMask, lhs, T)
              second_select = ComputeShuffleSelect(2, 0, z, w);
          } else {
              DCHECK(x >= 4);
              x -= 4;
              // T = (Rx Rx Ly Ly) = shufps(firstMask, lhs, rhs)
              first_select = ComputeShuffleSelect(x, x, y, y);
              // (Rx Ly Lz Lw) = (Tx Tz Lz Lw) = shufps(secondMask, lhs, T)
              second_select = ComputeShuffleSelect(0, 2, z, w);
          }

          __ movaps(temp, rhs);
          __ shufps(temp, lhs, first_select);
          __ shufps(temp, lhs, second_select);
          __ movaps(lhs, temp);
          return;
        } else if (num_lanes_from_lhs == 2) {
          if (x < 4 && y < 4) {
            uint8_t select = ComputeShuffleSelect(x, y, z % 4, w % 4);
            __ shufps(lhs, rhs, select);
            return;
          } else if (z < 4 && w < 4) {
            uint8_t select = ComputeShuffleSelect(x % 4, y % 4, z, w);
            __ movaps(temp, rhs);
            __ shufps(temp, lhs, select);
            __ movaps(lhs, temp);
            return;
          }

          // In two shufps, for the most generic case:
          uint8_t first_select[4], second_select[4];
          uint32_t i = 0, j = 2, k = 0;

#define COMPUTE_SELECT(lane)            \
          if (lane >= 4) {              \
            first_select[j] = lane % 4; \
            second_select[k++] = j++;   \
          } else {                      \
            first_select[i] = lane;     \
            second_select[k++] = i++;   \
          }

          COMPUTE_SELECT(x)
          COMPUTE_SELECT(y)
          COMPUTE_SELECT(z)
          COMPUTE_SELECT(w)
#undef COMPUTE_SELECT

          DCHECK(i == 2 && j == 4 && k == 4);

          int8_t select;
          select = ComputeShuffleSelect(first_select[0], first_select[1],
                                        first_select[2], first_select[3]);
          __ shufps(lhs, rhs, select);

          select = ComputeShuffleSelect(second_select[0], second_select[1],
                                        second_select[2], second_select[3]);
          __ shufps(lhs, lhs, select);
        }
        return;
      } else {
        Comment(";;; deoptimize: non-constant selector for shuffle");
        DeoptimizeIf(no_condition, instr, Deoptimizer::kForcedDeoptToRuntime);
        return;
      }
    }

    default:
      UNREACHABLE();
      return;
  }
}


#undef __

}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_IA32
