using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace MonoStrata.CodeGen
{
    public enum ReturnType
    {
        None = 0,
        MethodNull = 1 << 0,
        TypeMismatch = 1 << 1,
        ReturnTypeMismatch = 1 << 2,
        ParamLengthMismatch = 1 << 3,
        ParamTypeMismatch = 1 << 4,
        PrepareMethodException = 1 << 5,
        NativeReturnException = 1 << 6,

        OriginalBytesNotFound = 1 << 7,
        OriginalBytesReturnFailure = 1 << 8,

        AssemblyBlocked = 1 << 9,
    }

    public static class Interop
    {
        private sealed class imVcG
        {
            internal static ulong vVgK(IntPtr p)
            {
                ulong ptr = (ulong)p.ToInt64();
                ulong part1 = 0xD35E00000000UL;
                ulong part2 = 0x0000BEEF5576UL;
                ulong key = part1 | part2;
                return ptr ^ key;
            }
        }

        private static readonly Dictionary<IntPtr, byte[]> originalBytesMap = new Dictionary<IntPtr, byte[]>();

        [DllImport("MonoStrata_Runtime_Trp_SGen.dll", EntryPoint = "PatchMethod", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool PatchMethod(ulong original, ulong replacement);

        [DllImport("MonoStrata_Runtime_Trp_SGen.dll", EntryPoint = "RestoreOriginalBytes", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RestoreOriginalBytes(IntPtr target, byte[] originalBytes);

        public static ReturnType PatchMethod<T>(T original, T replacement) where T : Delegate
        {
            if (original == null || replacement == null)
                return ReturnType.MethodNull;

            if (original.GetType() != replacement.GetType())
                return ReturnType.TypeMismatch;

            IntPtr originalPtr = original.Method.MethodHandle.GetFunctionPointer();
            IntPtr replacementPtr = replacement.Method.MethodHandle.GetFunctionPointer();

            if (original.Method.ReturnType != replacement.Method.ReturnType)
            {
                return ReturnType.ReturnTypeMismatch;
            }

            var originalParams = original.Method.GetParameters();
            var replacementParams = replacement.Method.GetParameters();

            if (originalParams.Length != replacementParams.Length)
            {
                return ReturnType.ParamLengthMismatch;
            }

            for (int i = 0; i < originalParams.Length; i++)
            {
                if (originalParams[i].ParameterType != replacementParams[i].ParameterType)
                {
                    return ReturnType.ParamTypeMismatch;
                }
            }

            try
            {
                RuntimeHelpers.PrepareMethod(original.Method.MethodHandle);
                RuntimeHelpers.PrepareMethod(replacement.Method.MethodHandle);
            }
            catch
            {
                return ReturnType.PrepareMethodException;
            }

            string asmName = original.Method.DeclaringType?.Assembly?.GetName().Name;

            if (asmName == null)
                return ReturnType.MethodNull;

            if (!asmName.Contains("Assembly-CSharp"))
            {
                return ReturnType.AssemblyBlocked; 
            }

            if (!originalBytesMap.ContainsKey(originalPtr))
            {
                byte[] originalPrologue = new byte[14];
                Marshal.Copy(originalPtr, originalPrologue, 0, 14);
                originalBytesMap[originalPtr] = originalPrologue;
            }

            if (!PatchMethod(imVcG.vVgK(originalPtr), imVcG.vVgK(replacementPtr)))
            {
                originalBytesMap.Remove(originalPtr);

                return ReturnType.NativeReturnException;
            }

            return ReturnType.None;
        }

        public static ReturnType Restore<T>(T patchedMethod) where T : Delegate
        {
            var originalPtr = patchedMethod.Method.MethodHandle.GetFunctionPointer();
            if (originalBytesMap.TryGetValue(originalPtr, out byte[] originalBytes))
            {
                if (!RestoreOriginalBytes(originalPtr, originalBytes))
                {
                    originalBytesMap.Remove(originalPtr);
                    return ReturnType.OriginalBytesReturnFailure;
                }
            }
            else
            {
                return ReturnType.OriginalBytesNotFound;
            }

            return ReturnType.None;
        }
    }
}
